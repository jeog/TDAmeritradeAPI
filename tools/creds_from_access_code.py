#
# Copyright (C) 2019 Jonathon Ogden <jeog.dev@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see http://www.gnu.org/licenses.
#

from tdma_api import auth, clib
from urllib.parse import unquote
import argparse, os

parser = argparse.ArgumentParser()
parser.add_argument("credentials_path", metavar="credentials-path", nargs='?',help="destired location of encrypted credentials file", type=str)
parser.add_argument("client_id", metavar="client-id", nargs='?', help="Client ID used for obtaining access code", type=str)
parser.add_argument("access_code", metavar="access-code", nargs='?', help="access code retrieved from TDMA", type=str)
parser.add_argument("credentials_password", metavar="credentials-password", nargs='?',help="password used to encrypt/decrypt credentials file", type=str)
parser.add_argument("--redirect-uri", help="Redirect URI used for obtaining access code(if other than localhost: https://127.0.0.1)", type=str)
parser.add_argument("--extract-code-from-url", help="Provide the entire url string returned from the grant access/login redirect error page(containing an encoded 'code=' field) for parsing, instead of the pure access code", action="store_true" )
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

def parse_decode_url(url):
    if not url:
        raise Exception("empty url")
    parts = url.split("code=")
    if len(parts) < 2:
        raise Exception("invalid url; no 'code' parameter key")
    if len(parts) > 2:
        raise Exception("invalid url; 'code=' found more than once")
    if not parts[1]:
        raise Exception("invalid url; no 'code' parameter value")
    return unquote(parts[1])


def get_input_str(msg):
    for i in range(N_ATTEMPTS):
        r = input(msg)
        if r:
            return r
        print(" - FAILED: empty")
    return None


def get_code():
    if args.extract_code_from_url:
        for i in range(N_ATTEMPTS):
            url = input("The Entire (Encoded) URL: ")
            try:
                return parse_decode_url(url)
            except Exception as exc:
                print(" - FAILED to parse/decode url:", str(exc))
        return None
    elif not args.access_code:
        return get_input_str("Access Code: ")
    return args.access_code


def get_cred_path():
    for i in range(N_ATTEMPTS):
        path = input("Path of Credentials File In Existing Directory(e.g /home/abc/mycreds.creds): " )
        if not path:
            print(" - FAILED: empty path")
            continue
        if os.path.exists(path):
            yesno = input("file already exists, overwrite? (yes/no): ")
            if yesno == "yes":
                if os.access(os.path.dirname(path), os.W_OK):
                    return path
                else:
                    print(" - FAILED: can't write to file")
        else:
            try:
                open(path, 'wb')
                return path
            except OSError as exc:
                print(" - FAILED:", str(exc) )
    return None


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

    path = args.credentials_path
    if not path:
        path = get_cred_path()
    if not path:
        print("- FAILED creating credentials file")
        exit(1)

    cid = args.client_id
    if not cid:
        cid = get_input_str("Client ID / Consumer Key (e.g RANDOMCONSUMERTKEYCHARACTERS@AMER.OAUTHAP): ")
    if not cid:
        print("- FAILED getting Client ID / Consumer Key")
        exit(1)

    redirect = args.redirect_uri
    if not redirect:
        redirect = DEF_REDIRECT_URI

    code = get_code()
    if code:
        password = args.credentials_password
        if not args.credentials_password:
            password = get_input_str("Password for Credentials File: ")

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





