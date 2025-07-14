# File Upload and Detection Server

A lightweight C++ server that accepts file uploads via HTTP POST requests, detects the file type based on its binary signature (magic number), stores the file, and optionally parses CSV data for analysis and encoding.

## Features

- HTTP server with GET and POST support
- File upload handling with MIME type detection
- CSV parsing with DataFrame-style output
- Label encoding and one-hot encoding of uploaded CSV data
- Static file serving (e.g., `index.html` from `./public`)
- JSON-based API responses
- Logging for info, warnings, and errors

## Build Instructions

### Prerequisites

- C++17 compatible compiler
- CMake 3.10 or newer
- Windows platform (uses WinSock)

### Steps

```bash
git clone https://github.com/kanyutu707/dataplatform.git
cd yourproject
mkdir build && cd build
cmake ..
cmake --build .
```

### Running the server
- ./dataplatform

### Sample response
```{
  "status": "file saved",
  "filename": "./uploads/csv/upload_0.csv",
  "size": 1234
}```
