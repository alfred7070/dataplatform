#include "../headers/check.hpp"
#include "../headers/accept.hpp"
#include "../headers/convert.hpp"

Convert convert;

std::string Check::check_data(const std::string &path, const std::vector<char> &body)
{
    static int counter = 0;
    std::string extension = detect_file_type(body);

    std::string folder = "./uploads/" + extension;
    std::filesystem::create_directories(folder);
    std::string filename = folder + "/upload_" + std::to_string(counter++) + "." + extension;

    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open())
    {
        return acceptor.accept_data("{\"error\":\"Failed to write file\"}", "application/json", 500, "Internal Server Error");
    }
    ofs.write(body.data(), body.size());
    ofs.close();

    if (extension == "csv")
    {
        convert.read_csv(filename);
    }

    std::ostringstream oss;
    oss << "{ \"status\": \"file saved\", \"filename\": \"" << filename << "\", \"size\": " << body.size() << " }";
    return acceptor.accept_data(oss.str(), "application/json", 200, "OK");
}

std::string Check::detect_file_type(const std::vector<char> &body)
{
    if (body.empty()) return "bin";

    const unsigned char* data = reinterpret_cast<const unsigned char*>(body.data());
    size_t size = body.size();

    if (size >= 2 && data[0] == 0xFF && data[1] == 0xD8)
    {
        return "jpg";
    }
    if (size >= 8 && data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47 &&
        data[4] == 0x0D && data[5] == 0x0A && data[6] == 0x1A && data[7] == 0x0A)
    {
        return "png";
    }
    if (size >= 6 && data[0] == 0x47 && data[1] == 0x49 && data[2] == 0x46 && data[3] == 0x38 &&
        (data[4] == 0x37 || data[4] == 0x39) && data[5] == 0x61)
    {
        return "gif";
    }
    if (size >= 12 && data[0] == 0x52 && data[1] == 0x49 && data[2] == 0x46 && data[3] == 0x46 &&
        data[8] == 0x57 && data[9] == 0x45 && data[10] == 0x42 && data[11] == 0x50)
    {
        return "webp";
    }
    if (size >= 4 && data[0] == 0x42 && data[1] == 0x4D)
    {
        return "bmp";
    }
    if (size >= 4 && ((data[0] == 0x49 && data[1] == 0x49 && data[2] == 0x2A && data[3] == 0x00) ||
                      (data[0] == 0x4D && data[1] == 0x4D && data[2] == 0x00 && data[3] == 0x2A)))
    {
        return "tiff";
    }

    if (size >= 4 && data[0] == 0x25 && data[1] == 0x50 && data[2] == 0x44 && data[3] == 0x46)
    {
        return "pdf";
    }
    if (size >= 8 && data[0] == 0xD0 && data[1] == 0xCF && data[2] == 0x11 && data[3] == 0xE0 &&
        data[4] == 0xA1 && data[5] == 0xB1 && data[6] == 0x1A && data[7] == 0xE1)
    {
 
        std::string bodyStr(body.begin(), body.end());
        if (bodyStr.find("Microsoft Office Word") != std::string::npos ||
            bodyStr.find("Word.Document") != std::string::npos)
        {
            return "doc";
        }
        if (bodyStr.find("Microsoft Office Excel") != std::string::npos ||
            bodyStr.find("Excel.Sheet") != std::string::npos)
        {
            return "xls";
        }
        if (bodyStr.find("Microsoft Office PowerPoint") != std::string::npos ||
            bodyStr.find("PowerPoint.Show") != std::string::npos)
        {
            return "ppt";
        }
        return "ole"; 
    }

    if (size >= 4 && data[0] == 0x50 && data[1] == 0x4B && data[2] == 0x03 && data[3] == 0x04)
    {
        std::string bodyStr(body.begin(), body.end());
       
        if (bodyStr.find("word/document.xml") != std::string::npos)
        {
            return "docx";
        }
        if (bodyStr.find("xl/workbook.xml") != std::string::npos)
        {
            return "xlsx";
        }
        if (bodyStr.find("ppt/presentation.xml") != std::string::npos)
        {
            return "pptx";
        }
      
        if (bodyStr.find("mimetype") != std::string::npos)
        {
            if (bodyStr.find("application/vnd.oasis.opendocument.text") != std::string::npos)
                return "odt";
            if (bodyStr.find("application/vnd.oasis.opendocument.spreadsheet") != std::string::npos)
                return "ods";
            if (bodyStr.find("application/vnd.oasis.opendocument.presentation") != std::string::npos)
                return "odp";
        }
     
        if (bodyStr.find("META-INF/MANIFEST.MF") != std::string::npos)
        {
            return "jar";
        }
  
        if (bodyStr.find("AndroidManifest.xml") != std::string::npos)
        {
            return "apk";
        }
        
        return "zip";
    }

    if (size >= 3 && data[0] == 0x49 && data[1] == 0x44 && data[2] == 0x33)
    {
        return "mp3";
    }
    if (size >= 4 && data[0] == 0xFF && (data[1] & 0xE0) == 0xE0)
    {
        return "mp3"; 
    }
    if (size >= 12 && data[0] == 0x52 && data[1] == 0x49 && data[2] == 0x46 && data[3] == 0x46 &&
        data[8] == 0x57 && data[9] == 0x41 && data[10] == 0x56 && data[11] == 0x45)
    {
        return "wav";
    }
    if (size >= 4 && data[0] == 0x4F && data[1] == 0x67 && data[2] == 0x67 && data[3] == 0x53)
    {
        return "ogg";
    }
    if (size >= 8 && data[4] == 0x66 && data[5] == 0x74 && data[6] == 0x79 && data[7] == 0x70)
    {
        return "m4a";
    }

    if (size >= 12 && data[0] == 0x52 && data[1] == 0x49 && data[2] == 0x46 && data[3] == 0x46 &&
        data[8] == 0x41 && data[9] == 0x56 && data[10] == 0x49 && data[11] == 0x20)
    {
        return "avi";
    }
    if (size >= 8 && data[4] == 0x66 && data[5] == 0x74 && data[6] == 0x79 && data[7] == 0x70)
    {
        std::string ftyp(reinterpret_cast<const char*>(data + 8), std::min(size - 8, (size_t)4));
        if (ftyp.find("mp4") != std::string::npos || ftyp.find("isom") != std::string::npos)
            return "mp4";
        if (ftyp.find("qt") != std::string::npos)
            return "mov";
        return "mp4"; 
    }
    if (size >= 4 && data[0] == 0x1A && data[1] == 0x45 && data[2] == 0xDF && data[3] == 0xA3)
    {
        return "mkv";
    }

    if (size >= 6 && data[0] == 0x52 && data[1] == 0x61 && data[2] == 0x72 && data[3] == 0x21 &&
        data[4] == 0x1A && data[5] == 0x07)
    {
        return "rar";
    }
    if (size >= 3 && data[0] == 0x1F && data[1] == 0x8B && data[2] == 0x08)
    {
        return "gz";
    }
    if (size >= 4 && data[0] == 0x37 && data[1] == 0x7A && data[2] == 0xBC && data[3] == 0xAF)
    {
        return "7z";
    }
    if (size >= 6 && data[0] == 0x42 && data[1] == 0x5A && data[2] == 0x68 && 
        data[3] >= 0x31 && data[3] <= 0x39)
    {
        return "bz2";
    }

    // Executables
    if (size >= 2 && data[0] == 0x4D && data[1] == 0x5A)
    {
        return "exe";
    }
    if (size >= 4 && data[0] == 0x7F && data[1] == 0x45 && data[2] == 0x4C && data[3] == 0x46)
    {
        return "elf";
    }
    if (size >= 4 && ((data[0] == 0xFE && data[1] == 0xED && data[2] == 0xFA && data[3] == 0xCE) ||
                      (data[0] == 0xCE && data[1] == 0xFA && data[2] == 0xED && data[3] == 0xFE)))
    {
        return "macho";
    }

    if (detect_xml_content(body))
    {
        return "xml";
    }
    if (detect_json_content(body))
    {
        return "json";
    }
    if (detect_csv_content(body))
    {
        return "csv";
    }
    if (detect_html_content(body))
    {
        return "html";
    }
    if (detect_text_content(body))
    {
        return "txt";
    }

    return "bin"; 
}

bool Check::detect_csv_content(const std::vector<char> &body)
{
    size_t start = 0;
    if (body.size() >= 3 && (unsigned char)body[0] == 0xEF && 
        (unsigned char)body[1] == 0xBB && (unsigned char)body[2] == 0xBF)
    {
        start = 3;
    }

    std::string content(body.begin() + start, body.end());
    std::istringstream iss(content);
    std::string line;
    
    int line_count = 0;
    int comma_lines = 0;
    
    while (std::getline(iss, line) && line_count < 10)
    {
        if (line.find(',') != std::string::npos)
        {
            comma_lines++;
        }
        line_count++;
    }
    
    return line_count > 0 && (comma_lines * 2 > line_count);
}

bool Check::detect_json_content(const std::vector<char> &body)
{
    std::string content(body.begin(), body.end());
    size_t start = content.find_first_not_of(" \t\n\r");
    size_t end = content.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) return false;
    
    content = content.substr(start, end - start + 1);
    return (content.front() == '{' && content.back() == '}') ||
           (content.front() == '[' && content.back() == ']');
}

bool Check::detect_xml_content(const std::vector<char> &body)
{
    std::string content(body.begin(), body.end());
    size_t start = content.find_first_not_of(" \t\n\r");
    
    if (start == std::string::npos) return false;
    
    return content.substr(start, 5) == "<?xml" || content.substr(start, 1) == "<";
}

bool Check::detect_html_content(const std::vector<char> &body)
{
    std::string content(body.begin(), body.end());
    std::transform(content.begin(), content.end(), content.begin(), ::tolower);
    
    return content.find("<!doctype html") != std::string::npos ||
           content.find("<html") != std::string::npos ||
           content.find("<head") != std::string::npos ||
           content.find("<body") != std::string::npos;
}

bool Check::detect_text_content(const std::vector<char> &body)
{
   
    int printable_count = 0;
    int total_count = std::min((size_t)1000, body.size()); 
    
    for (int i = 0; i < total_count; i++)
    {
        unsigned char c = body[i];
        if ((c >= 32 && c <= 126) || c == '\t' || c == '\n' || c == '\r')
        {
            printable_count++;
        }
    }
    
    return total_count > 0 && (printable_count * 100 / total_count) > 80;
}