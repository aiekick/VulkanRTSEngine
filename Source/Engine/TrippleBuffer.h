#pragma once

// Declaration
template<typename T>
class TrippleBuffer
{
public:
	typedef array<T, 3> InternalBuffer;
	TrippleBuffer();

	void Advance();
	T& GetCurrent() { return buffers[currBuffer]; }

	InternalBuffer& GetInternalBuffer() { return buffers; }

private:
	InternalBuffer buffers;
	int currBuffer;
};

// Definition
template<typename T>
TrippleBuffer<T>::TrippleBuffer()
	: currBuffer(0)
{
}

template<typename T>
void TrippleBuffer<T>::Advance()
{
	currBuffer++;
	if (currBuffer == 3)
		currBuffer = 0;
}