#include "buffer.h"

Buffer::Buffer(int x) : buffer_(x), readPos_(0), writePos_(0) {};
size_t Buffer::WritableBytes() const
{
    return buffer_.size() - writePos_;
}
size_t Buffer::ReadableBytes() const
{
    return writePos_ - readPos_;
}
size_t Buffer::PrependableBytes() const
{
    return readPos_;
}
const char *Buffer::Peek() const
{
    return &buffer_[readPos_];
}
void Buffer::EnsureWriteable(size_t len)
{
    if (len > WritableBytes())
    {
        MakeSpace_(len);
    }
    assert(len <= WritableBytes());
}
void Buffer::HasWritten(size_t len)
{
    writePos_ += len;
}
void Buffer::Retrieve(size_t len)
{
    readPos_ += len;
}
void Buffer::RetrieveUntil(const char *end)
{
    assert(Peek() <= end);
    Retrieve(end - Peek());
}
void Buffer::RetrieveAll()
{
    memset(&buffer_[0], 0, buffer_.size());
    readPos_ = writePos_ = 0;
}
std::string Buffer::RetrieveAllToStr()
{
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}
const char *Buffer::BeginWriteConst() const
{
    return &buffer_[writePos_];
}
char *Buffer::BeginWrite()
{
    return &buffer_[writePos_];
}
void Buffer::Append(const char *str, size_t len)
{
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
}
void Buffer::Append(const std::string &str)
{
    Append(str.c_str(), str.size());
}
void Buffer::Append(const void *data, size_t len)
{
    Append(static_cast<const char *>(data), len);
}
void Buffer::Append(const Buffer &buff)
{
    Append(buff.Peek(), ReadableBytes());
}
ssize_t Buffer::ReadFd(int fd, int *Errno)
{
    char buff[65535];
    struct iovec iov[2];
    size_t writeable = WritableBytes();
    iov[0].iov_base = BeginWrite();
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);
    ssize_t len = readv(fd, iov, 2);
    if (len < 0)
    {
        *Errno = errno;
    }
    else if (static_cast<size_t>(len) <= writeable)
    {
        HasWritten(len);
    }
    else
    {
        writePos_ = buffer_.size();
        Append(buff, static_cast<size_t>(len - writeable));
    }
    return len;
}
ssize_t Buffer::WriteFd(int fd, int *Errno)
{
    ssize_t len = write(fd, Peek(), ReadableBytes());
    if (len < 0)
    {
        *Errno = errno;
    }
    Retrieve(len);
    return len;
}
char *Buffer::BeginPtr_()
{
    return &buffer_[0];
}
const char *Buffer::BeginPtr_() const
{
    return &buffer_[0];
}
void Buffer::MakeSpace_(size_t len)
{
    if (WritableBytes() + PrependableBytes() < len)
    {
        buffer_.resize(writePos_ + len + 1);
    }
    else
    {
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
        readPos_ = 0;
        writePos_ = readable;
        assert(readable == ReadableBytes());
    }
}