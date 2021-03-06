#pragma once

#include<cstring>
#include<vector>
#include<cstdint>

namespace whale_storm
{
	namespace base
	{
		class ByteArray :public std::vector<char>
		{
		public:
			ByteArray() = default;

			ByteArray(int32_t size):
				std::vector<char>(size) {}

			ByteArray(const char* buffer, int32_t size) :
				 std::vector<char>(buffer, buffer + size) {}

			std::string ToStdString() const
			{
				std::string result(this->cbegin(), this->cend());

				return result;
			}

			ByteArray& Concat(const ByteArray& buffer2)
			{
				size_t oldSize = size();
				size_t newSize = oldSize + buffer2.size();
				resize(newSize);
				memcpy(this->data() + oldSize, buffer2.data(), buffer2.size());

				return *this;
			}

			ByteArray operator+(const ByteArray& buffer2)const
			{
				ByteArray buffer1(this->size() + buffer2.size());
				memcpy(buffer1.data(), this->data(), this->size());
				memcpy(buffer1.data() + this->size(), buffer2.data(), buffer2.size());

				return buffer1;
			}
		};

		class IODevice
		{
		public:
			enum class SeekMode
			{
				Set,
				Forward,
				BackWard
			};

			~IODevice()
			{

			}
		};

		class ByteArrayReader :public IODevice
		{
		public:
			ByteArrayReader(const ByteArray& buffer) :
				_buffer(buffer), _pos(0) {}

			template<class T>
			int32_t Read(T* buffer, int32_t count)
			{
				if (_pos >= _buffer.size())
				{
					return 0;
				}

				int32_t sizeToRead = sizeof(T)*count;
				if (_pos + sizeToRead > _buffer.size())
				{
					sizeToRead = _buffer.size() - _pos;
				}

				memcpy(buffer, _pos + _buffer.data(), sizeToRead);
				_pos += sizeToRead;

				return sizeToRead;
			}

			template<class T>
			T Read()
			{
				T t;
				int32_t size = Read(&t, 1);

				return t;
			}

			ByteArray ReadData(int32_t size)
			{
				if (_pos >= _buffer.size())
				{
					return 0;
				}

				int32_t sizeToRead = size;
				if (_pos + sizeToRead > _buffer.size())
				{
					sizeToRead = _buffer.size() - _pos;
				}
				ByteArray result(sizeToRead);
				memcpy(result.data(), _buffer.data() + _pos, sizeToRead);
				_pos += sizeToRead;

				return result;
			}

			int32_t Tell() const
			{
				return _pos;
			}

			void Seek(SeekMode mode, int32_t size)
			{
				int32_t dest = _pos;
				if (mode == SeekMode::Set)
					dest = size;
				else if (mode == SeekMode::Forward)
					dest += size;
				else if (mode == SeekMode::BackWard)
					dest -= size;
			}

		private:
			ByteArray _buffer;
			int32_t _pos;
		};

		class ByteArrayWriter
		{
		public:
			ByteArrayWriter() {}

			template<class T>
			int32_t Write(const T* buffer, int32_t count)
			{
				int32_t sizeToWrite = sizeof(T)*count;
				ByteArray buffer2((const char*)(buffer), sizeToWrite);
				_buffer.Concat(buffer2);

				return sizeToWrite;
			}

			template<class T>
			int32_t Write(const T& value)
			{
				return Write(&value,1)
			}
			int32_t Write(const ByteArray& buffer)
			{
				_buffer.Concat(buffer);

				return buffer.size();
			}

			const ByteArray& ToByteArray() const
			{
				return _buffer;
			}

			int32_t Tell() const
			{
				return _buffer.size();
			}

		private:
			ByteArray _buffer;
		};
	}
}