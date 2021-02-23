#pragma once

#include <memory>

template<class T>
class DataBinder
{
public:
	void BindData(const std::weak_ptr<T>& config);
	void ResetBind();
	std::shared_ptr<T> GetData() const;
private:
	std::weak_ptr<T> m_data;
};

template<class T>
inline void DataBinder<T>::BindData(const std::weak_ptr<T>& config)
{
	m_data = config;
}

template<class T>
inline void DataBinder<T>::ResetBind()
{
	m_data.reset();
}

template<class T>
inline std::shared_ptr<T> DataBinder<T>::GetData() const
{
	return m_data.lock();
}