#ifndef __GAME_HPP__
#define __GAME_HPP__

class game
{
public:
    virtual void iterate() = 0;
    virtual void clear() = 0;
};

#endif // __GAME_HPP__
