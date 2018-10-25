#ifndef ___TESTWORKS__EXCEPTIONS__
#define ___TESTWORKS__EXCEPTIONS__

#include <string>

class TwException {
protected:
	TwException * _embeddedException;	
	std::string _reason;
public:
	TwException() {
		_embeddedException = NULL;
		_reason = "";
	}
	TwException(std::string reason) {
		_embeddedException = NULL;
		_reason = reason;
	}
	TwException(std::string reason, TwException * embeddedException) {
		_embeddedException = embeddedException;
		_reason = reason;
	}
	inline std::string getReason() {
		return _reason;
	}
	inline TwException * getEmbeddedException() {
		return _embeddedException;
	}
};

class TimeoutException : public TwException {
};

#endif

