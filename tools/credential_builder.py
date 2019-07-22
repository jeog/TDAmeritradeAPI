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

"""credential_builder.py - grant account access and build Credentials file

dependencies:
    tdma_api   :: python setup.py install (from project's python directory)
    cefpython3 :: pip install cefpython3

credential_builder.py will attempt to:
    1) Open a Web Browser using Chromium Embedded Framework
    2) Ask you to login to your TDMA account
    3) Ask you to grant permission to the app set up in your developer account
    4) Return an access code (to the redirect uri)
    5) Catch the redirect and use the access code to get access tokens
    6) Build a Credentials object
    7) Save and encrypt the Credentials object to disk

"""

from tdma_api import auth, clib
from cefpython3 import cefpython as cef
from urllib.parse import unquote, quote
import sys, argparse, os

parser = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)

parser.add_argument("client_id", metavar="client-id",
                    help="Client ID(Consumer Key with @AMER.OAUTHAP appended)",
                    type=str)
parser.add_argument("credentials_path", metavar="credentials-path",
                    help="desired location of encrypted credentials file",
                    type=str)
parser.add_argument("credentials_password", metavar="credentials-password",
                    help="password used to encrypt/decrypt credentials file",
                    type=str)
parser.add_argument("--redirect-uri",
                    help="Redirect URI used for obtaining access code" \
                        + "if other than localhost(https://127.0.0.1)",
                    type=str)
parser.add_argument("--no-confirm-password",
                    help="Don't make user re-enter password to confirm",
                    action="store_true")
parser.add_argument("--no-confirm-overwrite",
                    help="Don't make user confirm overwrite of credentials file",
                    action="store_true")
parser.add_argument("--no-check-id-suffix",
                    help="Disable check for @AMER.OAUTHAP at end of Client ID",
                    action="store_true")
parser.add_argument("--access-code-out",
                    help="Send access code to file, '-' for stdout", type=str)
parser.add_argument("--library-path", help="load C library from custom path",
                    type=str)


BASE_AUTH_URL = "https://auth.tdameritrade.com/auth?response_type=code"
DEF_REDIRECT_URI = "https://127.0.0.1"
CONSUMER_KEY_SUFFIX = "@AMER.OAUTHAP"
N_ATTEMPTS = 3
WINDOW_TITLE = "TDAmeritradeAPI Credential Builder"

class RequestHandler(object):
    def __init__(self, redirect_uri):
        self._redirect_uri = redirect_uri
        self.code = None
        self.error = ""

    def OnResourceRedirect(self, browser, frame, old_url, new_url_out, request,
                           response):
        for new_url in new_url_out:
            if self._redirect_uri in new_url:
                try:
                    self.code = self._parse_redirect_url(new_url)
                except ValueError as exc:
                    self.err = str(exc)
                finally:
                    browser.CloseBrowser(True)


    def _parse_redirect_url(self, url):
        if not url:
            raise ValueException("empty url")
        parts = url.split("code=")
        if len(parts) < 2:
            raise ValueException("invalid url; no 'code' parameter key")
        if len(parts) > 2:
            raise ValueException("invalid url; 'code=' found more than once")
        if not parts[1]:
            raise ValueException("invalid url; no 'code' parameter value")
        return unquote(parts[1])


class AccessCodeException(Exception):
    pass


def get_access_code_from_web_interface(client_id, redirect_uri ):
    addr = BASE_AUTH_URL + "&redirect_uri=" +  quote(redirect_uri) \
                + "&client_id=" + quote(client_id)
    old_hook = sys.excepthook
    sys.excepthook = cef.ExceptHook
    cef.Initialize()
    browser = cef.CreateBrowserSync(url=addr, window_title=WINDOW_TITLE)
    handler = RequestHandler(redirect_uri)
    browser.SetClientHandler(handler)
    cef.MessageLoop()
    cef.Shutdown()
    sys.excepthook = old_hook
    if not handler.code:
        raise AccessCodeException("failed to receive access code: " + handler.error)
    return handler.code


def build_credentials(code, client_id, redirect_uri):
    return auth.request_access_token(code, client_id, redirect_uri)


def store_credentials(path, password, creds):
     auth.store_credentials(path, password, creds)


def main():
    print(" + Extract Args...")
    try:
        lib_path, redirect_uri, client_id, cred_path, cred_password, out = _get_args()
    except ValueError as exc:
        print(" - FAILURE:", str(exc))
        return False

    print("   Library Path:", lib_path if lib_path else "N/A")
    print("   Redirect URI:", redirect_uri)
    print("   Client ID:", client_id)
    print("   Credential Path:", cred_path)
    print("   Credential Password:", cred_password[:2] + ((len(cred_password)-2) * '*'))

    if lib_path:
        try:
            clib.init(lib_path, reload=True)
        except Exception as exc:
            print(" - Error initializing C library:", str(exc))

    if not clib._lib:
        print(" - FAILURE: C library not loaded")
        return False

    try:
        code = get_access_code_from_web_interface(client_id, redirect_uri)
    except AccessCodeException as exc:
        print("- FAILURE: did not receive access code:", str(exc))
        return False

    if out is not None:
        print( str(code), file=out )

    try:
        creds = build_credentials(code, client_id, redirect_uri)
    except CLibException as exc:
        print(" - FAILURE: could not build credentials:", str(exc))
        return False

    try:
        store_credentials(cred_path, cred_password, creds)
    except CLibException as exc:
        print(" - FAILURE: could not store credentials:", str(exc))
        return False

    print(" + SUCCESS storing credentials to %s for %s" % (cred_path, client_id))
    return True


# returns (lib, redirect, id, path, password, code out file)
def _get_args():
    args = parser.parse_args()

    lib_path = args.library_path
    if lib_path and not os.path.exists(lib_path):
        raise ValueError("library path is not valid")

    redirect_uri = args.redirect_uri
    if not redirect_uri:
        redirect_uri = DEF_REDIRECT_URI

    client_id = args.client_id
    if not client_id:
        raise ValueError("empty client id")

    if not args.no_check_id_suffix:
        if not client_id.endswith(CONSUMER_KEY_SUFFIX):
            raise ValueError("client id doesn't end with '%s'" % CONSUMER_KEY_SUFFIX)

        if len(client_id) == len(CONSUMER_KEY_SUFFIX):
            raise ValueError("client id requires more than '%s'" % CONSUMER_KEY_SUFFIX)

    cred_password = args.credentials_password
    if not cred_password:
        raise ValueError("empty credentials password")

    def confirm_password(cred_password):
        for i in range(N_ATTEMPTS):
            pw = input(" + Confirm password: ")
            if pw == cred_password:
                return True
            print("  - Passwords don't match!")
        return False

    if not args.no_confirm_password:
        if not confirm_password(cred_password):
            raise ValueError("passwords don't match")

    cred_path = args.credentials_path
    if not cred_path:
        raise ValueError("empty credentials path")

    creds_exist = os.path.exists(cred_path)
    if creds_exist:
        if not args.no_confirm_overwrite:
            yesno = input(" + File already exists, overwrite? (yes/no): ")
            if yesno != "yes":
                print("  - Can't overwrite '%s', exiting..." % cred_path )
                exit()
    else:
        try:
            open(cred_path, 'wb')
        except IOError as exc:
            raise ValueError("can't open '%s': %s" % (cred_path,str(exc)))

    out = None
    if args.access_code_out:
        if args.access_code_out != '-':
            try:
                out = open(args.access_code_out,'w')
            except IOError as exc:
                if not creds_exist:
                    try:
                        os.remove(cred_path)
                    except:
                        pass
                raise ValueError("can't open '%s': %s" % (args.access_code_out,str(exc)))
        else:
            out = sys.stdout

    return(lib_path, redirect_uri, client_id, cred_path, cred_password, out)


if __name__ == '__main__':
    if not main():
        exit(1)



