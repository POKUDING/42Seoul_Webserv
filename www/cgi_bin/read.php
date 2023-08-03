#!/usr/bin/php
<?php

// 요청된 파일명(.php)을 가져옴
// $file = $_GET['file'];

// root 환경변수에 있는 파일인지 확인하기 위해 절대 경로 생성
// echo "|||$rootPath|||";
// $filePath = $rootPath . $file;

// // 파일명에 .php가 포함되어 있지 않으면 중지
// if (strpos($file, '.php') === false) {
    //     die("Invalid file.");
    // }
    
    // 파일이 존재하면 읽어서 출력
    // if (file_exists($filePath)) {
        // 보안상 취약한 점을 강조하기 위해 사용. 실제로는 피해야 함.
        // echo "Content of $file:<br>";
        // echo file_get_contents($filePath);
    // } else {
        // die("File not found.");
    // } 
    
$filePath = getenv('DOCUMENT_ROOT');
$command = "/usr/bin/php "; // 실행하고자 하는 명령어
$command .= $filePath;
$result = exec($command, $output, $returnVar);

if ($returnVar === 0) {
    // 명령어 실행 성공
    echo "Content-Type: text/html\n\n";
    echo implode($output);
} else {
    // 명령어 실행 실패
    echo "Command execution failed!";
}