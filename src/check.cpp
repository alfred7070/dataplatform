#include "../headers/check.hpp"
#include "../headers/accept.hpp"

std::string Check::check_data(const std::string &path, const std::vector<char> &body)
{
    static int counter = 0;

    std::string extension = "bin";

    if (body.size() >= 4)
    {
        if ((unsigned char)body[0] == 0xFF && (unsigned char)body[1] == 0xD8)
        {
            extension = "jpg";
        }
        else if (body[0] == 0xEF && body[1] == 0xBB && body[2] == 0xBF)
        {
            extension = "csv";
        }
        else if ((unsigned char)body[0] == 0x25 && (unsigned char)body[1] == 0x50 && (unsigned char)body[2] == 0x44 && (unsigned char)body[3] == 0x46)
        {
            extension = "pdf";
        }
        else if (body[0] == 0x50 && body[1] == 0x4B &&
                 body[2] == 0x03 && body[3] == 0x04)
        {
            std::string bodyStr(body.begin(), body.end());

            if (bodyStr.find("word/document.xml") != std::string::npos)
            {
                extension = "docx";
            }
            else if (bodyStr.find("xl/workbook.xml") != std::string::npos)
            {
                extension = "xlsx";
            }
            else
            {
                extension = "zip";
            }
        }
    }

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

    std::ostringstream oss;
    oss << "{ \"status\": \"file saved\", \"filename\": \"" << filename << "\", \"size\": " << body.size() << " }";
    return acceptor.accept_data(oss.str(), "application/json", 200, "OK");
}
