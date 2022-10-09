#include <Windows.h>
#include <exception>
#include <stdexcept>
#include <memory>
#include <string>
typedef std::basic_string<TCHAR> String;

String GetErrorMessage(DWORD dwErrorCode)
{
    LPTSTR psz = NULL;
    const DWORD cchMsg = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS
        | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, // (not used with FORMAT_MESSAGE_FROM_SYSTEM)
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&psz),
        0,
        NULL);
    if (cchMsg > 0)
    {
        // Assign buffer to smart pointer with custom deleter so that memory gets released
        // in case String's c'tor throws an exception.
        auto deleter = [](void* p) { ::HeapFree(::GetProcessHeap(), 0, p); };
        std::unique_ptr<TCHAR, decltype(deleter)> ptrBuffer(psz, deleter);
        return String(ptrBuffer.get(), cchMsg);
    }
    else
    {
        throw std::runtime_error("Failed to retrieve error message string.");
    }
}