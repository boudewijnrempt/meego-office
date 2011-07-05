#ifndef CM_SEARCHING_INTERFACE_H
#define CM_SEARCHING_INTERFACE_H

class QString;
class SearchingInterface
{
public:
    SearchingInterface() { }
    virtual ~SearchingInterface() { }

    virtual int matchCount() = 0;
    
    virtual void find(const QString& pattern) = 0;
    virtual void findPrevious() = 0;
    virtual void findNext() = 0;
    virtual void findFinished() = 0;

    virtual void findMatchFound(int match) = 0;
};

#endif //CM_SEARCHING_INTERFACE_H