
from tdma_api import auth, clib
import argparse
 
parser = argparse.ArgumentParser()
parser.add_argument("credentials_path", metavar="credentials-path", nargs='?',help="destired location of encrypted credentials file", type=str)
parser.add_argument("client_id", metavar="client-id", nargs='?', help="Client ID used for obtaining access code", type=str)
parser.add_argument("access_code", metavar="access-code", nargs='?', help="access code retrieved from TDMA", type=str)
parser.add_argument("credentials_password", metavar="credentials-password", nargs='?',help="password used to encrypt/decrypt credentials file", type=str)
parser.add_argument("--redirect-uri", help="Redirect URI used for obtaining access code(if other than localhost: https://127.0.0.1)", type=str)
args = parser.parse_args()


N_ATTEMPTS = 3
DEF_REDIRECT_URI = "https://127.0.0.1"


def create_credentials(code, cid, redirect, path, password):
    try:
        creds = auth.request_access_token(code, cid, redirect)           
    except Exception as exc:
        print("- failed to build credentials:", str(exc))
        return False

    try:
        auth.store_credentials(path, password, creds)
    except Exception as exc:
        print(" - failed to store credentials:", str(exc))
        return False
    
    return True
        

if __name__ == '__main__':

    print("* * *")
    if clib._lib is None:
        print("Need to load TDAmeritradeAPI library.")
        for i in range(N_ATTEMPTS):
            lib_path = input("Library Path: ")
            try:
                if clib.init(lib_path):
                    print(" + load/init SUCCESS")
                    break
                print("- load/init FAILURE")
            except Exception as exc:
                print("- load/init FAILURE:", str(exc))

    if clib._lib is None:
        print("- no library, exiting...")
        exit(1)

    if not args.credentials_path:
        path = input("Path of Credentials File(e.g /home/abc/mycreds.creds): " )
    else:
        path = args.credentials_path

    if not args.client_id:
        cid = input("Client ID(e.g CLIENT@AMER.OAUTHAP): ")
    else:
        cid = args.client_id

    if not args.access_code:
        code = input("Access Code: ")
    else:
        code = args.access_code

    if not args.redirect_uri:
        redirect = DEF_REDIRECT_URI
    else:
        redirect = args.redirect_uri        

    password = args.credentials_password
    if not args.credentials_password:
        password = input("Password for Credentials File: ")           

    for i in range(N_ATTEMPTS):
        password2 = input("Confirm Password for Credentials File: ")
        if password == password2:
            if create_credentials(code, cid, redirect, path, password):
                print("+ SUCCESS creating credentials file @ %s for %s" % (path, cid))
                exit()
            else:
                break            
        print("- passwords don't match!")
        
    print("- FAILED creating credentials file @ %s for %s" % (path,cid))
    exit(1)

    

    

