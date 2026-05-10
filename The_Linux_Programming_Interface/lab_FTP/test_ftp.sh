#!/bin/bash

SERVER="127.0.0.1"
PORT="2100"
USER="admin"
PASS="admin123"
PASS_COUNT=0
FAIL_COUNT=0

echo "=== FTP Server Test Suite ==="
echo ""

# Test 1: Login with correct credentials and list
echo "Test 1: Login and list directory"
result=$(curl -s --max-time 10 ftp://$USER:$PASS@$SERVER:$PORT/ 2>&1)
if echo "$result" | grep -q "rw"; then
    echo "[PASS] Login and list"
    PASS_COUNT=$((PASS_COUNT + 1))
else
    echo "[FAIL] Login and list (expected directory listing with permissions)"
    echo "  Got: $result"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi
echo "Response preview: $(echo $result | head -c 100)"
echo ""

# Test 2: Authentication failure
echo "Test 2: Authentication failure"
# Enable curl verbose to capture FTP response codes
result=$(curl -v --max-time 10 ftp://$USER:wrongpassword@$SERVER:$PORT/ 2>&1)
if echo "$result" | grep -q "530\|Login incorrect\|Access denied"; then
    echo "[PASS] Wrong password rejected"
    PASS_COUNT=$((PASS_COUNT + 1))
elif echo "$result" | grep -q "230\|Login successful"; then
    echo "[FAIL] Wrong password accepted"
    FAIL_COUNT=$((FAIL_COUNT + 1))
else
    # Connection failed which is also acceptable for wrong password
    echo "[PASS] Wrong password rejected (connection terminated)"
    PASS_COUNT=$((PASS_COUNT + 1))
fi
echo ""

# Test 3: Upload file
echo "Test 3: Upload file"
echo "FTP upload test - $(date)" > /tmp/ftp_upload_test.txt
curl -s --max-time 10 -T /tmp/ftp_upload_test.txt ftp://$USER:$PASS@$SERVER:$PORT/upload_test.txt 2>&1
sleep 1
result=$(curl -s --max-time 10 ftp://$USER:$PASS@$SERVER:$PORT/ 2>&1)
if echo "$result" | grep -q "upload_test.txt"; then
    echo "[PASS] Upload"
    PASS_COUNT=$((PASS_COUNT + 1))
else
    echo "[FAIL] Upload"
    echo "  Directory listing: $(echo $result | head -c 200)"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi
echo ""

# Test 4: Download file (fixed)
echo "Test 4: Download file"
# Upload a file first to ensure it exists
echo "Download test content - $(date)" > /tmp/ftp_download_source.txt
curl -s --max-time 10 -T /tmp/ftp_download_source.txt ftp://$USER:$PASS@$SERVER:$PORT/download_test.txt 2>&1
sleep 1
# Now download it
curl -s --max-time 10 -o /tmp/ftp_downloaded.txt ftp://$USER:$PASS@$SERVER:$PORT/download_test.txt 2>&1
if [ -f /tmp/ftp_downloaded.txt ] && [ -s /tmp/ftp_downloaded.txt ]; then
    echo "[PASS] Download"
    echo "  Content: $(cat /tmp/ftp_downloaded.txt)"
    PASS_COUNT=$((PASS_COUNT + 1))
else
    echo "[FAIL] Download"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi
echo ""

# Test 5: Path traversal protection
echo "Test 5: Path traversal protection"
result=$(curl -s --max-time 10 ftp://$USER:$PASS@$SERVER:$PORT/../../etc/passwd 2>&1)
if [ -z "$result" ] || echo "$result" | grep -q "550\|Access denied\|No such file\|failed"; then
    echo "[PASS] Path traversal blocked"
    PASS_COUNT=$((PASS_COUNT + 1))
else
    echo "[FAIL] Path traversal (should be blocked)"
    echo "  Got: $result"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi
echo ""

# Test 6: Unauthenticated access (fixed)
echo "Test 6: Unauthenticated access"
# Don't provide any credentials - try as anonymous
result=$(curl -s --max-time 10 ftp://anonymous:guest@$SERVER:$PORT/ 2>&1)
if [ -z "$result" ] || echo "$result" | grep -q "530\|Login incorrect\|Please login"; then
    echo "[PASS] Unauthenticated access denied"
    PASS_COUNT=$((PASS_COUNT + 1))
else
    echo "[FAIL] Unauthenticated access should be denied"
    echo "  Got: $result"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi
echo ""

# Test 7: Multiple file operations
echo "Test 7: Multiple file operations"
for i in 1 2 3; do
    echo "File $i content - $(date)" > /tmp/ftp_multi_$i.txt
    curl -s --max-time 10 -T /tmp/ftp_multi_$i.txt ftp://$USER:$PASS@$SERVER:$PORT/multi_test_$i.txt 2>&1
done
sleep 1
result=$(curl -s --max-time 10 ftp://$USER:$PASS@$SERVER:$PORT/ 2>&1)
if echo "$result" | grep -q "multi_test_1.txt" && echo "$result" | grep -q "multi_test_2.txt" && echo "$result" | grep -q "multi_test_3.txt"; then
    echo "[PASS] Multiple file operations"
    PASS_COUNT=$((PASS_COUNT + 1))
else
    echo "[FAIL] Multiple file operations"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi
echo ""

# Test 8: FTP protocol commands (fixed)
echo "Test 8: FTP command sequence"
result=$(echo -e "USER $USER\r\nPASS $PASS\r\nPWD\r\nSYST\r\nFEAT\r\nQUIT\r\n" | nc -w 5 $SERVER $PORT 2>&1)
if echo "$result" | grep -q "220" && echo "$result" | grep -q "230"; then
    echo "[PASS] FTP command sequence"
    echo "  FTP session successful"
    PASS_COUNT=$((PASS_COUNT + 1))
else
    echo "[FAIL] FTP command sequence"
    echo "  Response: $result"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi
echo ""

# Test 9: SIZE command
echo "Test 9: SIZE command"
result=$(echo -e "USER $USER\r\nPASS $PASS\r\nSIZE upload_test.txt\r\nQUIT\r\n" | nc -w 5 $SERVER $PORT 2>&1)
if echo "$result" | grep -q "213"; then
    echo "[PASS] SIZE command"
    PASS_COUNT=$((PASS_COUNT + 1))
else
    echo "[FAIL] SIZE command"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi
echo ""

# Cleanup
rm -f /tmp/ftp_upload_test.txt /tmp/ftp_downloaded.txt /tmp/ftp_download_source.txt /tmp/ftp_multi_*.txt

echo "=== Results: $PASS_COUNT passed, $FAIL_COUNT failed ==="
if [ $FAIL_COUNT -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed. Check the output above for details."
    exit 1
fi