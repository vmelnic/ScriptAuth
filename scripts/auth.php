<?php
/**
 * array(5) {
 * [0] =>
 * string(47) "/etc/3proxy/scripts/auth.php" // PHP script file
 * [1] =>
 * string(16) "username" // Username
 * [2] =>
 * string(9) "password" // Password
 * [3] =>
 * string(14) "127.0.0.1" // Proxy source host
 * [4] =>
 * string(4) "8080" // Proxy source port
 * }
 */

$url = ''; // Update $url to your API endpoint

$params = [
    'username' => $argv[1],
    'password' => $argv[2],
    'hostname' => $argv[3],
];

// Init curl request to your API endpoint
$ch = curl_init();

curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_HEADER, true);
curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_POSTFIELDS, $params);

$response = curl_exec($ch);

$httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

curl_close($ch);

if (FALSE === $response)
    exit(1);

if (200 !== $httpcode)
    exit(1);

// In order to validate/invalidate auth use exit() status codes.
// 0 - Success
// 1 - Fail

exit(0);
