// 禁止拷贝基类
#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

namespace zest
{
    
class noncopyable
{
public:
    noncopyable(const noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

} // namespace zest

#endif