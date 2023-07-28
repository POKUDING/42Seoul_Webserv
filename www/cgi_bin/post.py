import cgi
import os
import sys
import io

def save_uploaded_files(form):
    # 파일이 업로드되는 디렉토리 설정
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Create the 'upload' directory if it doesn't exist
    upload_dir = os.path.join(script_dir, 'upload')
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)

    # 업로드된 파일들 저장
    for key in form.keys():
        field = form[key]
        if isinstance(field, cgi.FieldStorage):
            if field.filename:
                file_path = os.path.join(upload_dir, os.path.basename(field.filename))

                with open(file_path, 'wb') as f:
                    while True:
                        chunk = field.file.read(8192)
                        if not chunk:
                            break
                        f.write(chunk)

if __name__ == "__main__":
    # 환경변수로부터 CONTENT_LENGTH와 CONTENT_TYPE을 읽어옵니다.
    content_length = os.environ.get('CONTENT_LENGTH', 0)
    content_type = os.environ.get('CONTENT_TYPE', '')

    # 환경변수에 설정된 값을 출력합니다.
    print(f"CONTENT_LENGTH: {content_length}")
    print(f"CONTENT_TYPE: {content_type}")

    # 표준 입력에서 바디 데이터를 읽어옵니다.
    body_data = sys.stdin.buffer.read(int(content_length))

    # 멀티파트 데이터 파싱
    form = cgi.FieldStorage(fp=io.BytesIO(body_data), environ={'REQUEST_METHOD': 'POST', 'CONTENT_TYPE': content_type})

    # 멀티파트 데이터 처리
    save_uploaded_files(form)

    # 업로드된 파일들을 저장한 디렉토리에 확인
    print("Uploaded files are saved in the 'upload' directory.")
