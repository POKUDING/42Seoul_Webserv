#!/usr/bin/env python3
import cgi
import os
import re
import sys

def parse_multipart_octet_stream():
    content_type = os.environ.get('CONTENT_TYPE', '')
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

    for part in parts[1:-1]:  # Skip the first and last parts (boundary markers)
        # Extract filename and content
        header, content = part.split(b'\r\n\r\n', 1)
        filename_match = re.search(r'filename="(.*?)"', header.decode(), re.DOTALL)
        if filename_match:
            filename = filename_match.group(1)
            filename = os.path.basename(filename)  # Remove any path information for security
            with open(filename, 'wb') as f:
                f.write(content)

if __name__ == "__main__":
    print("Content-Type: text/plain")
    print()

    try:
        parse_multipart_octet_stream()
        print("Files saved successfully.")
    except Exception as e:
        print(f"Error: {str(e)}")
