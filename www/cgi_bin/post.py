#!/usr/bin/env python3
import cgi
import os
import re
import sys


def parse_multipart_octet_stream():
    content_type = os.environ.get('CONTENT_TYPE', '')
    upload_dir = os.environ.get('DOCUMENT_ROOT', './var/www/html')

    if 'multipart/form-data' not in content_type:
        print("Content-Type: text/plain")
        print()
        print("Expected 'multipart/form-data' content type.")
        sys.exit(1)

    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length == 0:
        print("Content-Type: text/plain")
        print()
        print("No data received.")
        sys.exit(1)

    boundary = content_type.split('boundary=')[-1].encode('utf-8')

    # Read the raw POST data
    post_data = sys.stdin.buffer.read(content_length)

    # Split the data into individual parts based on the boundary
    parts = post_data.split(b'--' + boundary)

    # Create the upload_dir if it doesn't exist
    os.makedirs(upload_dir, exist_ok=True)
    for part in parts[1:-1]:  # Skip the first and last parts (boundary markers)
        # Extract filename and content
        header, content = part.split(b'\r\n\r\n', 1)
        filename_match = re.search(r'filename="(.*?)"', header.decode(), re.DOTALL)
        if filename_match:
            filename = filename_match.group(1)
            filename = os.path.basename(filename)  # Remove any path information for security
            file_path = os.path.join(upload_dir, filename)

            with open(file_path, 'wb') as f:
                f.write(content)

content = """
<html>
<head><title>CGI Example</title></head>
<body>
<h1>POST SUCCES!!</h1>
</body>
</html>
"""

if __name__ == "__main__":

    try:
        parse_multipart_octet_stream()
    except Exception as e:
        print(f"Error: {str(e)}")
        sys.exit(1)


    content_length = len(content)

    print("HTTP/1.1 201 OK")
    print("Content-Length:", content_length)
    print("Connection: keep-alive")
    
    print("Content-Type: text/html")
    print()  # 헤더와 본문을 구분하는 빈 줄

    print(content)
    print()

    sys.exit(0)