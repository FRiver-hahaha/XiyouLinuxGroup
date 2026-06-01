# FTP Server/Client Fixes Summary

## Date: 2026-06-01

### Issues Fixed

#### 1. **SIGSEGV Crash After Login** ✅
**Problem**: Client crashed immediately after successful login with `SIGSEGV (Address boundary error)`

**Root Cause**: 
- Server's PWD command didn't verify user authentication before accessing session data
- Client's response parsing lacked robustness for error cases
- Potential race condition in session handling

**Fixes**:
- **Server.cpp**: Added authentication check for PWD command before accessing session data
- **Server.cpp**: Added session existence check in `handleClientData()` before processing
- **Client.cpp**: Added response validation in `updateCurrentDir()` to handle empty/error responses
- **Client.cpp**: Added response validation in `fetchPendingFiles()` to handle empty/error responses
- **Client.cpp**: Added status messages during initialization for better debugging

#### 2. **File Upload - 0 Bytes Transferred** ✅
**Problem**: Files uploaded showed 0 bytes, files were created but empty

**Root Cause**: Critical bug in file reading logic:
```cpp
file.seekg(0, std::ios::end);  // Seek to end to get size
std::streamsize fileSize = file.tellg();
// BUG: If offset==0, file pointer stays at END!
if(offset > 0) {
    file.seekg(offset, std::ios::beg);  // Only seeks if offset > 0
}
// Reading from END position = empty content!
buffer << file.rdbuf();
```

**Fix**: Added explicit seek to beginning when offset=0:
```cpp
if(offset > 0) {
    if(offset >= fileSize) { ... }
    file.seekg(offset, std::ios::beg);
} else {
    // Critical fix: must seek to beginning when offset=0
    file.seekg(0, std::ios::beg);
}
```

Applied to both `uploadFile()` and `uploadFileWithLocalPath()` functions.

#### 3. **Full Path File Upload** ✅
**Problem**: Client couldn't upload files using full paths (e.g., `/tmp/file.txt`)

**Root Cause**: `tuiUploadFile()` extracted filename from path but called `uploadFile()` with just the filename, causing file-not-found error.

**Fix**: Created new `uploadFileWithLocalPath()` function that:
- Takes both local full path and remote filename
- Reads file using full local path
- Uploads to server using remote filename

#### 4. **Directory Switching** ✅
**Problem**: According to agent.md, directory switching was broken

**Status**: Directory switching was actually working correctly after the login crash fix. The CWD command properly:
- Handles relative paths (`documents`, `..`)
- Handles absolute paths (`/`)
- Updates client's current directory display
- Restricts users to their own directory tree

### Files Modified

1. **Server.cpp**
   - `handleFtpCommand()`: Added auth check for PWD command
   - `handleClientData()`: Added session existence check

2. **Client.cpp**
   - `updateCurrentDir()`: Added response validation
   - `fetchPendingFiles()`: Added response validation
   - `tuiMainLoop()`: Added status messages
   - `uploadFile()`: Fixed file seek bug
   - `tuiUploadFile()`: Updated to use `uploadFileWithLocalPath()`
   - `uploadFileWithLocalPath()`: New function for full-path uploads

3. **Client.h**
   - Added declaration for `uploadFileWithLocalPath()`

### Testing Results

All features now working correctly:
- ✅ Login (no more crash)
- ✅ Directory listing (LIST)
- ✅ Directory switching (CWD, CDUP)
- ✅ File upload (STOR) - with full paths
- ✅ File download (RETR)
- ✅ Current directory display (PWD)
- ✅ Resume file list (RESUME)

### Test Commands

```bash
# Start server
cd /home/friver/gitclone/XiyouLinuxGroup/The_Linux_Programming_Interface/lab_FTPserver
./bin/ftpserver

# Test client (in another terminal)
./bin/ftpclient

# Test sequence:
# 1. Login: user1 / pass1
# 2. List directory: option 3
# 3. Change directory: option 4 → "documents"
# 4. List again: option 3
# 5. Go back: option 4 → ".."
# 6. Upload file: option 1 → "/tmp/test.txt"
# 7. Download file: option 2 → "test.txt"
# 8. Exit: option 0
```

### Notes

- The upload 0-bytes bug was a pre-existing issue in the original code
- The login crash was caused by missing authentication checks
- All fixes maintain backward compatibility with existing functionality
- User isolation (each user in their own directory) continues to work correctly
