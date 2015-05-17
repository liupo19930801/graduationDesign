/*************************************************************************
	> File Name: LogFilter.cpp
	> Author:liupo
	> Mail:liupopublic@163.com
	> Created Time: Sat 18 Apr 2015 12:26:23 AM CST
 ************************************************************************/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>

using namespace std;

#define ENDOFFILE 0x0
#define GDB_NEXT "(gdb) next"
#define GDB_LOCAL "(gdb) info local"
#define GDB_SOURCE "(gdb) info source"
#define GDB_RUN "(gdb) run > /dev/null"

const int BUFFERSIZE = 1024+5;

class LogFilter
{
public:
    LogFilter();
    LogFilter(const char*, const char* );
    ~LogFilter();
    void doFileter();
private:
    const char* logfilename;
    const char* newfilename;
    ifstream* ifstream_pointer;
    ofstream* ofstream_pointer;

    bool openFileForRead();
    bool openFileForWrite();

    bool closeFileForRead();
    bool closeFileForWrite();

    char* getNextLine();
    bool writeLineToFile(const char*);

    bool IsSucceededOpenLogFile();
    bool IsEndofTheLogFile();

    bool handleEntities();

    bool handleSoureInfo();
    bool handleRunEntity();
    bool handleEntity();

    bool handleNextSource();
    bool handleNextInfo();

    bool handleFile();
    bool handleDirection();
    bool handleLocation();
    bool handleLines();
    bool handleLanguage();
    bool handleDebugFormat();

    int lastSpace(const char *);

protected:

};

LogFilter::LogFilter():logfilename("log"), newfilename("newlog")
{
    this->ifstream_pointer = NULL;
    this->ofstream_pointer = NULL;
}
LogFilter::LogFilter(const char* logfilename, const char* newfilename):logfilename(logfilename), newfilename(newfilename)
{
    this->ifstream_pointer = NULL;
    this->ofstream_pointer = NULL;
}
LogFilter::~LogFilter()
{
    if (this->ifstream_pointer != NULL)
    {
            delete this->ifstream_pointer;
            this->ifstream_pointer = NULL;
    }
    if (this->ofstream_pointer != NULL)
    {
        delete this->ofstream_pointer;
        this->ofstream_pointer = NULL;
    }
}
bool LogFilter::openFileForRead()
{
    this->ifstream_pointer = new ifstream(logfilename);
    if(this->ifstream_pointer == NULL)
        return false;
    return true;
}
bool LogFilter::openFileForWrite()
{
    this->ofstream_pointer = new ofstream(newfilename);
    if(this->ofstream_pointer == NULL)
        return false;
    return true;
}
bool LogFilter::closeFileForRead()
{
    this->ifstream_pointer->close();
    return true;
}
bool LogFilter::closeFileForWrite()
{
    this->ofstream_pointer->close();
    return true;
}
char* LogFilter::getNextLine()
{
    if(IsEndofTheLogFile())
        return ENDOFFILE;

    char *buffer = new char[BUFFERSIZE];
    this->ifstream_pointer->getline(buffer, BUFFERSIZE);
    return buffer;
}
bool LogFilter::writeLineToFile(const char* buffer)
{
    (*this->ofstream_pointer)<<buffer<<endl;
    //
    //取消delete，因为传进的参数可能是常量字符串
    //
    //delete []buffer;
    return true;
}
bool LogFilter::IsSucceededOpenLogFile()
{
    return this->ifstream_pointer->good();
}
bool LogFilter::IsEndofTheLogFile()
{
    return this->ifstream_pointer->eof();
}
bool LogFilter::handleNextInfo()
{
    if(IsEndofTheLogFile()) return false;

    writeLineToFile("===> INFO:");
    while(true)
    {
        const char *buffer = getNextLine();

        if(buffer == ENDOFFILE) return false;

        if(strcmp(buffer, GDB_NEXT) == 0)
        {
            delete []buffer;
            break;
        }
        writeLineToFile(buffer);
        delete []buffer;
    }
    return true;
}
bool LogFilter::handleNextSource()
{
    if(IsEndofTheLogFile()) return false;

    writeLineToFile("===> POSITION:");
    while(true)
    {
        char *buffer = getNextLine();
        
        if(buffer == ENDOFFILE) return false;

        if(strcmp(buffer, GDB_LOCAL) == 0)
        {
            delete []buffer;
            break;
        }
        else if(( buffer[0] == '\0')
                || (buffer[BUFFERSIZE-1] = buffer[11],
                        buffer[11] = '\0',
                        buffer[BUFFERSIZE-2] = strcmp(buffer, "Breakpoint "),
                        buffer[11]=buffer[BUFFERSIZE-1],
                        buffer[BUFFERSIZE-2]==0))
        {
            delete []buffer;
            continue;
        }
        else if(buffer[BUFFERSIZE-1] = buffer[18],
               buffer[18] = '\0',
               buffer[BUFFERSIZE-2] = strcmp(buffer, "__libc_start_main "),
               buffer[18] = buffer[BUFFERSIZE-1],
               buffer[BUFFERSIZE-2] == 0)
        {
            writeLineToFile("[:--end of file--:]");
            return false;
        }
        writeLineToFile(buffer);
        delete []buffer;
    }
    return true;
}
bool LogFilter::handleEntity()
{
    return handleNextSource()&&handleNextInfo();
}
bool LogFilter::handleRunEntity()
{
    while(true)
    {
        const char *buffer = getNextLine();

        if(buffer == ENDOFFILE) return false;

        if(strcmp(buffer, GDB_RUN) == 0)
        {
            delete []buffer;
            break;
        }
    }
    getNextLine(); getNextLine();
    writeLineToFile("===> POSITION:");
    while(true)
    {
        char *buffer = getNextLine();

        if(buffer == ENDOFFILE) return false;

        if(strcmp(buffer, GDB_SOURCE) == 0)
        {
            delete []buffer;
            break;
        }
        else if((buffer[0] == '\0') 
               || (buffer[BUFFERSIZE-1] = buffer[11],
                   buffer[11] = '\0',
                   buffer[BUFFERSIZE-2]=strcmp(buffer,"Breakpoint "),
                   buffer[11]=buffer[BUFFERSIZE-1],
                   buffer[BUFFERSIZE-2] == 0 ))
        {
            delete []buffer;
            continue;
        }
        writeLineToFile(buffer);
        delete []buffer;
    }
    while(true)
    {
        const char *buffer = getNextLine();

        if(buffer == ENDOFFILE) return false;

        if(strcmp(buffer, GDB_LOCAL) == 0)
        {
            delete []buffer;
            break;
        }
        delete []buffer;
    }
    writeLineToFile("===> INFO");
    while(true)
    {
        const char *buffer = getNextLine();

        if(buffer == ENDOFFILE) return false;

        if(strcmp(buffer, GDB_NEXT) == 0)
        {
            delete []buffer;
            break;
        }
        writeLineToFile(buffer);
        delete []buffer;
    }
    return true;
}
int LogFilter::lastSpace(const char* str)
{
    int len = strlen(str);
    for(int i = len-1; i >= 0; --i)
    {
        if(str[i] == ' ')
            return i;
    }
    return 0;
}
bool LogFilter::handleFile()
{
    char *buffer = new char[BUFFERSIZE];
    this->ifstream_pointer->getline(buffer, BUFFERSIZE);
    int last = lastSpace(buffer);
    writeLineToFile(buffer+last+1);
    delete []buffer;
    return true;
}
bool LogFilter::handleDirection()
{
    char *buffer = new char[BUFFERSIZE];
    this->ifstream_pointer->getline(buffer, BUFFERSIZE);
    int last = lastSpace(buffer);
    writeLineToFile(buffer+last+1);
    delete []buffer;
    return true;
}
bool LogFilter::handleLocation()
{
    char *buffer = new char[BUFFERSIZE];
    this->ifstream_pointer->getline(buffer, BUFFERSIZE);
    int last = lastSpace(buffer);
    writeLineToFile(buffer+last+1);
    delete []buffer;
    return true;
}
bool LogFilter::handleLines()
{
    char *buffer = new char[BUFFERSIZE];
    this->ifstream_pointer->getline(buffer, BUFFERSIZE);

    int last = lastSpace(buffer);
    buffer[last] = 0;

    last = lastSpace(buffer);
    writeLineToFile(buffer+last+1);

    delete []buffer;
    return true;
}
bool LogFilter::handleLanguage()
{
    char *buffer = new char[BUFFERSIZE];
    this->ifstream_pointer->getline(buffer, BUFFERSIZE);
    int last = lastSpace(buffer);
    writeLineToFile(buffer+last+1);
    delete []buffer;
    return true;
}
bool LogFilter::handleDebugFormat()
{
    writeLineToFile("DWARF 2");
    return true;
}

bool LogFilter::handleSoureInfo()
{

    char *buffer = new char[BUFFERSIZE];
    while(true)
    {
        this->ifstream_pointer->getline(buffer, BUFFERSIZE);
        if(strcmp(buffer, GDB_SOURCE) == 0)
        {
            break;
        }
    }
    delete []buffer;
    writeLineToFile("===> FILE:");
    handleFile();
    writeLineToFile("===> DIRECTION:");
    handleDirection();
    writeLineToFile("===> LOCATION:");
    handleLocation();
    writeLineToFile("===> LINES:");
    handleLines();
    writeLineToFile("===> LANGUAGE:");
    handleLanguage();
    writeLineToFile("===> DEBUG_FORMAT:");
    handleDebugFormat();
    return true;
}

bool LogFilter::handleEntities()
{
    this->openFileForRead();
    this->openFileForWrite();
    writeLineToFile("####################################################################");
    writeLineToFile("#HELLO,BODY,WELCOME USE MY LOG_DEBUG_TOOL!                         #");
    writeLineToFile("#@AUTHOR:LiuPo                                                     #");
    writeLineToFile("#@VERESION:1.0                                                     #");
    writeLineToFile("#@EMAIL:liupopublic@163.com                                        #");
    writeLineToFile("#@WEIBO:primo_001                                                  #");
    writeLineToFile("####################################################################");

    writeLineToFile("\n\n\n");
    writeLineToFile("Partion-1ST:");
    writeLineToFile("--------------------------------------------------------------------");

    //写入文件信息:FILE:{}
    writeLineToFile("\nFILE{\n");
    handleSoureInfo();
    writeLineToFile("\n}\n");
    this->ifstream_pointer->seekg(0, ios_base::beg);

    writeLineToFile("--------------------------------------------------------------------"); 


    writeLineToFile("\n\n\n");
    writeLineToFile("Partion-2ND:");
    writeLineToFile("--------------------------------------------------------------------");

    //写入运行信息:RUN:{}
    writeLineToFile("\nRUN:{\n");
    //循环写入各条语句运行信息
    handleRunEntity();
    while(handleEntity()){}
    writeLineToFile("\n}\n");

    writeLineToFile("--------------------------------------------------------------------");
    

    this->closeFileForRead();
    this->closeFileForWrite();

    return true;
}
void LogFilter::doFileter()
{
    handleEntities();
}

/**
**该程序需要两个参数
**第一个是待处理的日志文件名
**第二个是处理后的输出文件名
**/

int main(int argc, char** argv)
{
    char *logfilename = argv[1];
    char *newfilename = argv[2];
    LogFilter* logfilter = new LogFilter(logfilename, newfilename);
    logfilter->doFileter();

    return 0;
}


