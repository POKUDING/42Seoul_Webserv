#!/usr/bin/env python3
import cgi
import os
import re
import sys


if __name__ == "__main__":

    try:
        filename = os.environ.get('DOCUMENT_ROOT', '')
        os.system("python " + filename)
    except Exception as e:
        print(f"Error: {str(e)}")
        sys.exit(1)
        
