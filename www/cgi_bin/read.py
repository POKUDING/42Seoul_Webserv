#!/usr/bin/env python3
import cgi
import os
import re
import sys

content_type = ""
body = ""

def read_python_file():
    filename = os.environ.get('DOCUMENT_ROOT', '')
    head_flag = 1
    with open(filename, 'r') as file:
        string = file.readline()
        if head_flag:
            if len(string) and string.startswith("Content-Type: "):
                if len(content_type) == 0:
                    content_type = string
                else:#중복키
                    sys.exit(1)
            elif len(string) == 0:
                head_flag = 0
        else:
            body += string


if __name__ == "__main__":

    try:
        read_python_file()
    except Exception as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

    if len(content_type) == 0:
        content_type = "Content-Type: text/plain"

    print(content_type)
    print()  # 헤더와 본문을 구분하는 빈 줄
    print(body)
