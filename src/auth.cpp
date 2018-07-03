/*
Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses.
*/

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <chrono>
#include <random>

#include "../include/_tdma_api.h"

#include "openssl/evp.h"
#include "openssl/conf.h"

std::string certificate_bundle_path;
/*
 * Empty certificate_bundle_path means we let curl use the default store. 
 * If that works w/ openssl great, otherwise (if client doesn't set it
 * w/ SetCertificateBundlePath() ) we'll fail w/ CURLE_SSL_CACERT and throw 
 */

namespace tdma{

/* this should all be done during build */
#ifdef _WIN32
const std::string DEF_CERTIFICATE_BUNDLE_PATH(std::string(TDMA_API_ROOT_DIR) + "\\cacert.pem");
#elif defined(__GNUC__)
const std::string DEF_CERTIFICATE_BUNDLE_PATH(
    std::string(__FILE__).substr(0, std::string(__FILE__).rfind("/include/"))
    + "/cacert.pem"
);
#else
    #warning "'DEF_CERTIFICATE_BUNDLE_PATH' not defined!"
#endif

/*
 *  encrypted credentials file: IV + BODY + (IV + BODY CHECKSUM)
 *
 *  NOTE - FORMAT WAS CHANGED IN THE COMMIT FOLLOWING b75586

 *  IV (PLAIN TEXT, 16 bytes)

 *  BODY (ENCRYPTED/BINARY, n bytes)
 *       access token (string)
 *       \n
 *       refresh token (string)
 *       \n
 *       epoch sec (string)
 *       \n
 *       client id (string)
 *       \n
 *       plaintext body checksum (binary, 32 bytes)
 *       \n
 *
 *  IV + BODY CHECKSUM (binary, 32 bytes)
 */

using namespace std;
using namespace conn;

const string URL_ACCESS_TOKEN = URL_BASE + "oauth2/token";

const vector<pair<string,string>> AUTH_HEADERS = {
    {"Content-Type", "application/x-www-form-urlencoded"}
};

const int CREDS_IV_LENGTH = 16;
const int CREDS_CHECKSUM_LENGTH = 32;

const long TOKEN_EXPIRATION_MARGIN_SEC = 60 *60 * 24; // 1 day

/* this is a bit arbitrary, (just trying to avoid junk) */
const long TOKEN_EARLIEST_EXPIRATION = 1528323136;
const long long TOKEN_LATEST_EXPIRATION =
        TOKEN_EARLIEST_EXPIRATION + (60ULL * 60 * 24 * 365 * 100); // 100 yrs


template<typename T>
class SmartBuffer{
    vector<T> _buffer;

public:
    SmartBuffer() {}

    SmartBuffer(size_t n)
        : _buffer(n)
        {}

    SmartBuffer(const SmartBuffer& sb)
        : _buffer(sb._buffer)
        {}

    SmartBuffer&
    operator=(const SmartBuffer<T>& sb)
    {
        if( _buffer != sb._buffer )
            _buffer = sb._buffer;
        return *this;
    }

    SmartBuffer(SmartBuffer<T>&& sb)
        : _buffer(move(sb._buffer))
        {}

    SmartBuffer&
    operator=(SmartBuffer<T>&& sb)
    {
        _buffer = move(sb._buffer);
        return *this;
    }

    explicit SmartBuffer(const vector<T>& v)
        : _buffer(v)
        {}

    template<typename A>
    explicit SmartBuffer(const basic_string<A>& s)
        :
            _buffer()
    {
        for(auto ss : s)
            _buffer.push_back( static_cast<T>(ss) );        
    }


    template<typename A, size_t SZ>
    explicit SmartBuffer(const A (&array)[SZ])
        :
            _buffer()
    {
        for(size_t i = 0; i < SZ; ++i)
            _buffer.push_back(static_cast<T>(array[i]));
    }

    virtual
    ~SmartBuffer(){}

    typename vector<T>::const_iterator
    begin() const 
    { return _buffer.begin(); }

    typename vector<T>::const_iterator
    end() const
    { return _buffer.end(); }
    
    template<typename A>
    SmartBuffer(const SmartBuffer<A>& sb)
        : _buffer()
    {
        for(auto s : sb)
            _buffer.push_back(static_cast<T>(s));
    }

    SmartBuffer
    operator+(const SmartBuffer& sb) const
    {
        SmartBuffer<T> tmp(_buffer);      
        tmp._buffer.insert(tmp._buffer.end(), sb._buffer.begin(), sb._buffer.end());
        return tmp;
    }

    bool
    operator==(const SmartBuffer& sb) const
    { return _buffer == sb._buffer; }

    bool
    operator!=(const SmartBuffer& sb) const
    { return !operator==(sb); }

    T*
    get()
    { return &(*_buffer.begin()); }

    size_t
    size() const
    { return _buffer.size(); }

    void
    resize(size_t n)
    { _buffer.resize(n); }

    void
    push_back(T v)
    { _buffer.push_back(v); }

    SmartBuffer
    sub_buffer(size_t pos, size_t n)
    { return SmartBuffer(vector<T>(_buffer.begin() + pos, _buffer.begin() + pos + n)); }

    SmartBuffer
    sub_buffer(size_t pos)
    { return SmartBuffer(vector<T>(_buffer.begin() + pos, _buffer.end())); }
};
typedef SmartBuffer<unsigned char> SmartByteBuffer;


SmartByteBuffer
hash_sha256(SmartByteBuffer& in)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_create();
    if( !ctx )
        throw LocalCredentialException("failed to create digest context");

    if( EVP_DigestInit(ctx, EVP_sha256()) != 1 )
        throw LocalCredentialException("failed to init digest");
    
    if( EVP_DigestUpdate(ctx, in.get(), in.size()) != 1)
        throw LocalCredentialException("failed to update digest");

    int sz = EVP_MD_size(EVP_sha256());
    assert( sz > 0 );
    SmartByteBuffer out(sz);
    unsigned int outlen;
    if( EVP_DigestFinal(ctx, out.get(), &outlen) != 1 ){
        EVP_MD_CTX_destroy(ctx);
        throw LocalCredentialException("failed to get digest");
    }
    assert( outlen == static_cast<unsigned int>(sz) );

    EVP_MD_CTX_destroy(ctx);
    return out;
}

SmartByteBuffer
hash_sha256(const std::string& in)
{
    SmartByteBuffer buf(in);
    return hash_sha256(buf);
}


// <IV, ciphertext>
pair<SmartByteBuffer, SmartByteBuffer>
encrypt(std::string in, std::string key)
{
    std::default_random_engine engine( (std::random_device())() );
    /* msvc complains if we use unsigned char */
    std::uniform_int_distribution<unsigned int> distr(0,255);

    SmartByteBuffer iv;
    for(int i = 0; i < CREDS_IV_LENGTH; ++i){
        iv.push_back( static_cast<unsigned char>(distr(engine))  );
    }
    SmartByteBuffer hashed_key = hash_sha256(key);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if( !ctx )
        throw LocalCredentialException("failed to create cipher context");

    if( EVP_EncryptInit(ctx, EVP_aes_256_cbc(), hashed_key.get(), iv.get()) != 1 )
        throw LocalCredentialException("failed to init credential encryption");

    SmartByteBuffer in_b(in);
    SmartByteBuffer result(in_b.size() + CREDS_IV_LENGTH);

    int result_len, len;
    if( EVP_EncryptUpdate(ctx, result.get(), &len, in_b.get(), in_b.size()) != 1 ){
        EVP_CIPHER_CTX_free(ctx);
        throw LocalCredentialException("failed to update credential encryption");
    }
    result_len = len;

    if( EVP_EncryptFinal(ctx, result.get() + len, &len) != 1 ){
        EVP_CIPHER_CTX_free(ctx);
        throw LocalCredentialException("failed to finalize credential encryption");
    }
    result.resize(result_len + len);

    EVP_CIPHER_CTX_free(ctx);
    return make_pair(iv, result);
}


class DecryptFinalException
    : public LocalCredentialException {
};

SmartByteBuffer
decrypt(SmartByteBuffer in, SmartByteBuffer iv, string key)
{
    if( in.size() <= CREDS_IV_LENGTH )
        throw LocalCredentialException("not enough input to decrypt");

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if( !ctx )
        throw LocalCredentialException("failed to create cipher context");

    SmartByteBuffer hashed_key = hash_sha256(key);

    if( EVP_DecryptInit(ctx, EVP_aes_256_cbc(), hashed_key.get(), iv.get()) != 1){
        EVP_CIPHER_CTX_free(ctx);
        throw LocalCredentialException("failed to init credential decryption");
    }

    SmartByteBuffer result(in.size());
    int result_len, len;
    if( EVP_DecryptUpdate(ctx, result.get(), &len, in.get(), in.size() ) != 1){
        EVP_CIPHER_CTX_free(ctx);
        throw LocalCredentialException("failed to update credential decryption");
    }
    result_len = len;

    if( EVP_DecryptFinal(ctx, result.get() + len, &len) != 1){
        EVP_CIPHER_CTX_free(ctx);
        throw DecryptFinalException();
    }
    result.resize(result_len + len);
    result.push_back(0);

    EVP_CIPHER_CTX_free(ctx);
    return result;
}


bool
store_credentials(string path, string password, const Credentials& creds)
{
    fstream file(path, ios_base::out | ios_base::trunc | ios_base::binary );
    if( !file.is_open() ){
        throw LocalCredentialException("no credentials file at " + path);
    }
    file.exceptions(ios_base::badbit | ios_base::failbit);

    stringstream input;
    input << creds.access_token << endl
          << creds.refresh_token << endl
          << to_string(creds.epoch_sec_token_expiration) << endl
          << creds.client_id << endl;
    string input_str = input.str();

    auto input_checksum = hash_sha256(input_str);   
    input.write(reinterpret_cast<const char*>(input_checksum.get()),
                input_checksum.size());
    input << endl;
    input_str = input.str();

    SmartByteBuffer iv;
    SmartByteBuffer ctext;    
    tie(iv, ctext) = encrypt(input_str, password);

    auto iv_and_ctext= iv + ctext;
    auto iv_body_checksum = hash_sha256(iv_and_ctext);

    try{
        file.write(reinterpret_cast<const char*>(iv.get()), iv.size());
        file.write(reinterpret_cast<const char*>(ctext.get()), ctext.size());
        file.write(reinterpret_cast<const char*>(iv_body_checksum.get()),
                   iv_body_checksum.size());
        return true;
    }catch( ios_base::failure& f ){
        cerr<< "failed to store credentials in " + path << endl
            << "  " << f.what() << endl;
        return false;
    }
}


Credentials
load_credentials(fstream& file, string path, string password)
{      
    string f_str;
    try{
        file.exceptions(ios_base::eofbit | ios_base::badbit | ios_base::failbit);
        f_str = string( (std::istreambuf_iterator<char>(file)), 
                         std::istreambuf_iterator<char>() );
    }catch (ios_base::failure& f) {
        throw LocalCredentialException( "failed to load credentials from " + path
                                        + " - ios_base::failure - " + f.what() );        
    }

    if( f_str.size() <= (CREDS_IV_LENGTH + CREDS_CHECKSUM_LENGTH) ){
        throw LocalCredentialException("failed to load credentials from " 
                                        + path + " - input too small");
    }

    SmartByteBuffer input(f_str);
    auto civ = input.sub_buffer(0, CREDS_IV_LENGTH);
    size_t body_sz = input.size() - CREDS_IV_LENGTH - CREDS_CHECKSUM_LENGTH;             
    auto ctext = input.sub_buffer(CREDS_IV_LENGTH, body_sz);
    auto cchecksum = input.sub_buffer(CREDS_IV_LENGTH + body_sz);

    auto civ_and_ctext = civ + ctext;
    if( hash_sha256(civ_and_ctext) != cchecksum)
        throw LocalCredentialException("corrupted credentials file(checksum");        

    SmartByteBuffer dbody;
    try {
        dbody = decrypt(ctext, civ, password);
    }catch( LocalCredentialException& ) {
        cerr << "failed to decrypt credential file: " << path << endl;
        throw;
    }            
            
    Credentials creds;
    SmartByteBuffer derived_checksum, bchecksum;
    try{
        stringstream ss;
        ss.write(reinterpret_cast<const char*>(dbody.get()), dbody.size());
        ss.exceptions(ios_base::eofbit | ios_base::badbit | ios_base::failbit);
        
        getline(ss, creds.access_token);
        getline(ss, creds.refresh_token);
        string tmp;
        getline(ss, tmp);
        creds.epoch_sec_token_expiration = std::stoll(tmp);
        getline(ss, creds.client_id); 
        
        char bchecksum_str[CREDS_CHECKSUM_LENGTH];
        ss.read(bchecksum_str, CREDS_CHECKSUM_LENGTH);
        bchecksum = SmartByteBuffer(bchecksum_str);
        assert(!ss.eof()); // terminating new line

        ss.str("");
        ss << creds.access_token << endl
           << creds.refresh_token << endl
           << tmp << endl
           << creds.client_id << endl;
        derived_checksum = hash_sha256(ss.str());

    }catch( ios_base::failure& f ){
        throw LocalCredentialException( string("failed to read credentials")
                                        + " - ios_base::failure - " + f.what());
    }

    // this will fail in decrypt before we even get here  
    if( bchecksum.size() == 0 || derived_checksum != bchecksum ) {
        /*
        cerr << "derived checksum: ";
        for (auto e : derived_checksum)
        cerr << (int)e << ' ';
        cerr << endl;
        cerr << "body checksum: ";
        for (auto e : bchecksum)
        cerr << (int)e << ' ';
        cerr << endl;
        */
        throw LocalCredentialException("invalid credentials(checksum)");
    }

    if( !store_credentials(path + ".backup", password, creds) )
        cerr << "failed to write backup credentials file" << endl;

    return creds;
}


Credentials
LoadCredentials(string path, string password)
{
    fstream file(path, ios_base::in | ios_base::binary );
    if( file.is_open() ){
        try {
            return load_credentials(file, path, password);

        }catch (DecryptFinalException& e) {
            /* if bad password don't try the backup */
            throw LocalCredentialException("BAD PASSWORD");

        }catch (LocalCredentialException& e) {
            cerr << "failed to load primary credentials file: " << endl
                 << "  LocalCredentialsException caught: " << e.what() << endl;
        }
    }else {
        cerr << "no credentials file at " + path << endl;
    }

    string path2(path + ".backup");
    cerr<< "trying backup credentials file at " + path2 << endl;

    fstream file2(path2, ios_base::in | ios_base::binary);
    if (!file2.is_open()) 
        throw LocalCredentialException("no backup credentials file at " + path2);
    
    try {
        return load_credentials(file2, path2, password);
    }catch (DecryptFinalException&) {
        throw LocalCredentialException("BAD PASSWORD");
    }
}


bool
copy_credentials_file(string from_path, string to_path)
{
    fstream fout(to_path, ios_base::out | ios_base::trunc | ios_base::binary);
    if( !fout.is_open() ) {
        cerr << "no credentials file at " + to_path << endl;
        return false;
    }

    fstream fin(from_path, ios_base::in | ios_base::binary );
    if (!fin.is_open()) {
        cerr << "no credentials file at " + from_path << endl;
        return false;
    }
    
    fout.exceptions(ios_base::badbit | ios_base::failbit);
    fin.exceptions(ios_base::eofbit | ios_base::badbit | ios_base::failbit);
    try{
        fout<< string( (std::istreambuf_iterator<char>(fin)),
                        std::istreambuf_iterator<char>() );
    }catch( ios_base::failure& f ){
        cerr << "failed to copy credentials from " << from_path 
             << " to " << to_path << " - ios_base::failure - " << f.what();   
        return false;
    }

    return true;
}


void
StoreCredentials(string path, string password, const Credentials& creds)
{
    if( !store_credentials(path, password, creds) ){
        cerr << "  " << "revert to " << path + ".backup" << endl;
        /*
         * If initial store attempt fails from a write error just try to 
         * overwrite w/ backup. Allow LocalCredentialExceptions to
         * propogate from store_credentials() or copy_credentials_file()
         * since there's the store op is beyond saving at that point.
         */
        if( !copy_credentials_file(path + ".backup", path) )
            throw LocalCredentialException("failed to store credentials");
    }
}


bool
request_token_has_expired(long long epoch_sec_token_expiration)
{
    using namespace chrono;

    assert(epoch_sec_token_expiration > 0);
    auto now = system_clock::now();
    seconds sse = duration_cast<seconds>( now.time_since_epoch() );
    long long time_remaining = epoch_sec_token_expiration - sse.count();

    return (time_remaining < TOKEN_EXPIRATION_MARGIN_SEC);
}


long long
generate_token_expiration_sec(long life)
{
    using namespace chrono;

    assert(life > 0);
    auto now = system_clock::now();
    seconds sse = duration_cast<seconds>( now.time_since_epoch() );
    return sse.count() + life;
}


Credentials
RequestAccessToken(string code, string client_id, string redirect_uri)
{
    if( code.empty() )
        throw LocalCredentialException("'code' is empty");

    if( client_id.empty() )
        throw LocalCredentialException("'client_id' required");         

    HTTPSPostConnection connection(URL_ACCESS_TOKEN);
    connection.ADD_headers(AUTH_HEADERS);

    vector<pair<string, string>> fields = {
        {"grant_type","authorization_code"},
        {"access_type", "offline"},
        {"code", util::url_encode(code)},
        {"client_id", util::url_encode(client_id)},
        {"redirect_uri", util::url_encode(redirect_uri)}
    };
    connection.SET_fields(fields);

    auto r_json = api_auth_execute(connection, "RequestAccessToken");

    Credentials creds;
    creds.client_id = client_id;
    creds.access_token = r_json["access_token"];
    creds.refresh_token = r_json["refresh_token"];

    creds.epoch_sec_token_expiration =
            generate_token_expiration_sec(r_json["refresh_token_expires_in"]);

    if( creds.epoch_sec_token_expiration < TOKEN_EARLIEST_EXPIRATION ||
        creds.epoch_sec_token_expiration > TOKEN_LATEST_EXPIRATION )
    {
        throw LocalCredentialException("creds.epoch_sec_toke_expiration "
                                       "contains invalid value.");
    };
    return creds;
}


void
RefreshAccessToken(Credentials& creds) 
{
    if( creds.epoch_sec_token_expiration < TOKEN_EARLIEST_EXPIRATION ||
        creds.epoch_sec_token_expiration > TOKEN_LATEST_EXPIRATION )
    {
        string e( to_string(creds.epoch_sec_token_expiration) );
        throw LocalCredentialException("creds.epoch_sec_toke_expiration "
                                        "contains invalid value (" + e + ")" );
    };

    if( request_token_has_expired(creds.epoch_sec_token_expiration) )
    {
        /*
         *  TODO implement mechanism to automatically auth new token
         */
        throw LocalCredentialException("request token has expired; "
                                       "use RequestAccessToken() for new token");
    }

    if( creds.refresh_token.empty() )
        throw LocalCredentialException("creds.refresh_token is empty");

    HTTPSPostConnection connection(URL_ACCESS_TOKEN);
    connection.ADD_headers(AUTH_HEADERS);

    vector<pair<string, string>> fields = {
        {"grant_type","refresh_token"},
        {"refresh_token", util::url_encode(creds.refresh_token)},
        {"access_type", ""},
        {"code",""},
        {"client_id", util::url_encode(creds.client_id)},
        {"redirect_uri", ""}
    };
    connection.SET_fields(fields);

    auto r_json = api_auth_execute(connection, "RefreshAccessToken");
    creds.access_token = r_json["access_token"];
    if( creds.access_token.empty() ){
        throw LocalCredentialException("creds.access_token is empty");
    }
}


void
SetCertificateBundlePath(const std::string& path)
{    
    size_t sz = path.size(); 
    // .pem suffix with at least one char before
    if (sz < 5 || path[sz - 1] != 'm' || path[sz - 2] != 'e'
               || path[sz - 3] != 'p' || path[sz - 4] != '.')
    {
        throw ValueException("certicate bundle path is not valid '.pem' file path");
    }

    if( !fstream(path).is_open() )
        throw ValueException("invalid certificate bundle file");

    certificate_bundle_path = path;
}


} /* tdma */
