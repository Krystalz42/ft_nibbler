#ifndef PROJECT_SNAKENETWORK_HPP
#define PROJECT_SNAKENETWORK_HPP

class ISnakeNetwork {
public:
	virtual bool isOpen() const = 0;
	virtual void notifyBorderless() = 0;
	virtual void notifyMapSize() = 0;
	virtual const SnakeArrayContainer &getSnakeArray_() const = 0;
	virtual bool allSnakeIsDead() const = 0;
	virtual bool allSnakeIsReady() const = 0;
	~ISnakeNetwork() {};

};

#endif //PROJECT_SNAKENETWORK_HPP
