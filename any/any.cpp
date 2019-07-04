#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

#include <boost/any.hpp>

class NonCopyConstructible {
public:
	NonCopyConstructible(int, int) { }
	NonCopyConstructible(NonCopyConstructible&&) = delete;
	NonCopyConstructible(const NonCopyConstructible&) = delete;
	NonCopyConstructible& operator=(const NonCopyConstructible&) = delete;
	NonCopyConstructible& operator=(NonCopyConstructible&&) = delete;
};

class Listener {
public:
	virtual void notify(const boost::any& item) = 0;
	virtual ~Listener() {}
};

class ListenerImpl : public Listener {
	
	enum class TypeId : unsigned char{
		eString,
		eCharPointer,
		eInteger,
		eDouble
	};

	std::map<TypeId, std::function<void(const boost::any&)>> m_HandlersStorage;

public:

	ListenerImpl() {
		m_HandlersStorage[TypeId::eDouble] = [=](const boost::any& data) {
			this->DoubleHandler(data);
		};
		m_HandlersStorage[TypeId::eInteger] = [=](const boost::any& data) {
			this->IntHandler(data);
		};
		m_HandlersStorage[TypeId::eString] = [=](const boost::any& data) {
			this->StringHandler(data);
		};
		m_HandlersStorage[TypeId::eCharPointer] = [=](const boost::any& data) {
			this->CharPointerHandler(data);
		};
	}

	virtual void notify(const boost::any& data) {

		if (IsConcreteType<double>(data))
		{
			m_HandlersStorage[TypeId::eDouble](data);
		}
		else if (IsConcreteType<int>(data))
		{
			m_HandlersStorage[TypeId::eInteger](data);
		}
		else if (IsConcreteType<std::string>(data))
		{
			m_HandlersStorage[TypeId::eString](data);
		}
		else if (IsConcreteType<const char*>(data))
		{
			m_HandlersStorage[TypeId::eCharPointer](data);
		}
	}

private:

	template<class Type>
	bool IsConcreteType(const boost::any& data)
	{
		return data.type() == typeid(Type);
	}

	void IntHandler(const boost::any& data) {
		auto nInteger = boost::any_cast<int>(data);
		std::cout << "IntHandler(): " << nInteger << std::endl;
	}

	void DoubleHandler(const boost::any& data) {
		auto fDouble = boost::any_cast<double>(data);
		std::cout << "DoubleHandler(): " << fDouble << std::endl;
	}

	void StringHandler(const boost::any& data) {
		auto sString = boost::any_cast<std::string>(data);
		std::cout << "StringHandler(): " << sString << std::endl;
	}

	void CharPointerHandler(const boost::any& data) {
		auto sString = boost::any_cast<const char*>(data);
		std::cout << "CharPointerHandler(): " << sString << std::endl;
	}
};

class Notifier {
	std::weak_ptr<Listener> m_pListener;

public:
	Notifier(std::shared_ptr<Listener> pListener) : m_pListener(pListener) {

	}

	template<class Type>
	void ApplyData(const Type& type) {

		auto pListener = m_pListener.lock();

		if (pListener) {
			pListener->notify(type);
		}
	}
};
int main()
{
	std::vector<boost::any> vElements;
	vElements.push_back(1.15);
	vElements.push_back(std::string("String: Hello world!"));
	vElements.push_back("Ptr: Hello world!");
	vElements.push_back(42);

	//vElements.emplace_back(NonCopyConstructible(1, 4)); // not available

	try {
		auto nInteger = boost::any_cast<int>(vElements.back());
		std::cout << "Casted integer = " << nInteger << std::endl;
		vElements.pop_back();
	}
	catch (boost::bad_any_cast& ex){
		std::cout << "Bad any cast to interger: " << ex.what() << std::endl;
	}

	try {
		auto pChar = boost::any_cast<const char*>(vElements.back());
		std::cout << "Casted const char* = " << pChar << std::endl;
		vElements.pop_back();
	}
	catch (boost::bad_any_cast& ex) {
		std::cout << "Bad any cast to const char*: " << ex.what() << std::endl;
	}

	try {
		auto sString = boost::any_cast<std::string>(vElements.back());
		std::cout << "Casted std::string = " << sString << std::endl;
		vElements.pop_back();
	}
	catch (boost::bad_any_cast& ex) {
		std::cout << "Bad any cast to std::string : " << ex.what() << std::endl;
	}

	// Throw bad any cast here
	try {
		auto sString = boost::any_cast<std::string>(vElements.back());
		vElements.pop_back();
	}
	catch (boost::bad_any_cast& ex) {
		std::cout << "Bad any cast to std::string : " << ex.what() << std::endl;
	}

	std::shared_ptr<Listener> pListener{ std::make_shared<ListenerImpl>() };
	Notifier notifier { pListener };
	notifier.ApplyData(5);
	notifier.ApplyData(5.7);
	notifier.ApplyData(std::string("Hello world!"));
	notifier.ApplyData("Hello world!");

	return 0;
}
