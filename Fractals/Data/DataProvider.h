#pragma once

#include "DataBinder.h"

template<class T>
class DataProvider
{
public:
	DataProvider(std::shared_ptr<T>& data);
	void ProvideData(DataBinder<T>& binder);
private:
	std::shared_ptr<T>& m_data;
};

template<class T>
inline DataProvider<T>::DataProvider(std::shared_ptr<T>& data)
	: m_data(data)
{
}

template<class T>
inline void DataProvider<T>::ProvideData(DataBinder<T>& binder)
{
	binder.BindData(m_data);
}