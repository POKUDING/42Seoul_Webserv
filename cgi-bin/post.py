import cgi
import os

def save_uploaded_files(form):
    # 파일이 업로드되는 디렉토리 설정
    upload_dir = '../uploads/'

    # 업로드된 파일들 저장
    for key in form.keys():
        field = form[key]
        if isinstance(field, cgi.FieldStorage) and field.filename:
            file_path = os.path.join(upload_dir, os.path.basename(field.filename))

            with open(file_path, 'wb') as f:
                while True:
                    chunk = field.file.read(8192)
                    if not chunk:
                        break
                    f.write(chunk)

if __name__ == "__main__":
    # HTTP POST 요청 처리pypipasdfoija
    form = cgi.FieldStorage()

    # 멀티파트 데이터 파싱 후 파일 저장
    save_uploaded_files(form)

    # 업로드된 파일들을 저장한 디렉토리에 확인
    print("Uploaded files are saved in the 'uploads' directory.")
