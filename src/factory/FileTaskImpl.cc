/*
  Copyright (c) 2019 Sogou, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Authors: Li Jinghao (lijinghao@sogou-inc.com)
*/

#include <fcntl.h>
#include <unistd.h>
#include <string>
#include "WFGlobal.h"
#include "WFTaskFactory.h"

class WFFilepreadTask : public WFFileIOTask
{
public:
	WFFilepreadTask(int fd, void *buf, size_t count, off_t offset,
					IOService *service, fio_callback_t&& cb) :
		WFFileIOTask(service, std::move(cb))
	{
		this->args.fd = fd;
		this->args.buf = buf;
		this->args.count = count;
		this->args.offset = offset;
	}

	virtual int prepare()
	{
		this->prep_pread(this->args.fd, this->args.buf, this->args.count,
						 this->args.offset);
		return 0;
	}
};

class __WFFilepreadTask : public WFFilepreadTask
{
public:
	__WFFilepreadTask(const std::string &filename, void *buf, size_t count, off_t offset,
					  IOService *service, fio_callback_t&& cb):
		WFFilepreadTask(-1, buf, count, offset, service, std::move(cb))
	{
		this->filename = filename;
		this->fd = -1;
	}

	virtual int prepare()
	{
		this->fd = open(this->filename.c_str(), O_RDONLY);
		if (fd < 0)
		{
			this->error = errno;
			return -1;
		}
		this->prep_pread(this->fd, this->args.buf, this->args.count,
						 this->args.offset);
		return 0;
	}

protected:
	virtual SubTask *done()
	{
		if (this->fd >= 0)
		{
			close(this->fd);
		}
		return WFFileTask::done();
	}

private:
	std::string filename;
	int fd;
};

class WFFilepwriteTask : public WFFileIOTask
{
public:
	WFFilepwriteTask(int fd, const void *buf, size_t count, off_t offset,
					 IOService *service, fio_callback_t&& cb) :
		WFFileIOTask(service, std::move(cb))
	{
		this->args.fd = fd;
		this->args.buf = (void *)buf;
		this->args.count = count;
		this->args.offset = offset;
	}

	virtual int prepare()
	{
		this->prep_pwrite(this->args.fd, this->args.buf, this->args.count,
						  this->args.offset);
		return 0;
	}
};

class __WFFilepwriteTask : public WFFilepwriteTask
{
public:
	__WFFilepwriteTask(const std::string &filename, const void *buf, size_t count,
					  off_t offset, IOService *service, fio_callback_t&& cb):
		WFFilepwriteTask(-1, buf, count, offset, service, std::move(cb))
	{
		this->filename = filename;
		this->fd = -1;
	}

	virtual int prepare()
	{
		this->fd = open(this->filename.c_str(), O_WRONLY | O_CREAT);
		if (fd < 0)
		{
			this->error = errno;
			return -1;
		}
		this->prep_pwrite(this->fd, this->args.buf, this->args.count,
						  this->args.offset);
		return 0;
	}

protected:
	virtual SubTask *done()
	{
		if (this->fd >= 0)
		{
			close(this->fd);
		}
		return WFFileTask::done();
	}

private:
	std::string filename;
	int fd;
};

class WFFilepreadvTask : public WFFileVIOTask
{
public:
	WFFilepreadvTask(int fd, const struct iovec *iov, int iovcnt, off_t offset,
					 IOService *service, fvio_callback_t&& cb) :
		WFFileVIOTask(service, std::move(cb))
	{
		this->args.fd = fd;
		this->args.iov = iov;
		this->args.iovcnt = iovcnt;
		this->args.offset = offset;
	}

	virtual int prepare()
	{
		this->prep_preadv(this->args.fd, this->args.iov, this->args.iovcnt,
						  this->args.offset);
		return 0;
	}
};

class __WFFilepreadvTask : public WFFilepreadvTask
{
public:
	__WFFilepreadvTask(const std::string& filename, const struct iovec *iov,
					   int iovcnt, off_t offset, IOService *service,
					   fvio_callback_t&& cb) :
		WFFilepreadvTask(-1, iov, iovcnt, offset, service, std::move(cb))
	{
		this->filename = filename;
	}

	virtual int prepare()
	{
		this->fd = open(this->filename.c_str(), O_RDONLY);
		if (fd < 0)
		{
			this->error = errno;
			return -1;
		}
		this->prep_preadv(this->fd, this->args.iov, this->args.iovcnt,
						  this->args.offset);
		return 0;
	}

protected:
	virtual SubTask *done()
	{
		if (this->fd >= 0)
		{
			close(this->fd);
		}
		return WFFileTask::done();
	}

private:
	std::string filename;
	int fd;
};

class WFFilepwritevTask : public WFFileVIOTask
{
public:
	WFFilepwritevTask(int fd, const struct iovec *iov, int iovcnt, off_t offset,
					  IOService *service, fvio_callback_t&& cb) :
		WFFileVIOTask(service, std::move(cb))
	{
		this->args.fd = fd;
		this->args.iov = iov;
		this->args.iovcnt = iovcnt;
		this->args.offset = offset;
	}

	virtual int prepare()
	{
		this->prep_pwritev(this->args.fd, this->args.iov, this->args.iovcnt,
						   this->args.offset);
		return 0;
	}
};

class __WFFilepwritevTask : public WFFilepwritevTask
{
public:
	__WFFilepwritevTask(const std::string& filename, const struct iovec *iov,
					   int iovcnt, off_t offset, IOService *service,
					   fvio_callback_t&& cb) :
		WFFilepwritevTask(-1, iov, iovcnt, offset, service, std::move(cb))
	{
		this->filename = filename;
	}

	virtual int prepare()
	{
		this->fd = open(this->filename.c_str(), O_WRONLY);
		if (fd < 0)
		{
			this->error = errno;
			return -1;
		}
		this->prep_pwritev(this->fd, this->args.iov, this->args.iovcnt,
						  this->args.offset);
		return 0;
	}

protected:
	virtual SubTask *done()
	{
		if (this->fd >= 0)
		{
			close(this->fd);
		}
		return WFFileTask::done();
	}

private:
	std::string filename;
	int fd;
};

class WFFilefsyncTask : public WFFileSyncTask
{
public:
	WFFilefsyncTask(int fd, IOService *service, fsync_callback_t&& cb) :
		WFFileSyncTask(service, std::move(cb))
	{
		this->args.fd = fd;
	}

	virtual int prepare()
	{
		this->prep_fsync(this->args.fd);
		return 0;
	}
};

class __WFFilefsyncTask : public WFFilefsyncTask
{
public:
	__WFFilefsyncTask(const std::string& filename, IOService *service,
					  fsync_callback_t&& cb) :
		WFFilefsyncTask(-1, service, std::move(cb))
	{
		this->filename = filename;
	}

	virtual int prepare()
	{
		this->fd = open(this->filename.c_str(), O_WRONLY);
		if (fd < 0)
		{
			this->error = errno;
			return -1;
		}
		this->prep_fsync(this->args.fd);
		return 0;
	}

protected:
	virtual SubTask *done()
	{
		if (this->fd >= 0)
		{
			close(this->fd);
		}
		return WFFileTask::done();
	}

private:
	std::string filename;
	int fd;
};

class WFFilefdsyncTask : public WFFileSyncTask
{
public:
	WFFilefdsyncTask(int fd, IOService *service, fsync_callback_t&& cb) :
		WFFileSyncTask(service, std::move(cb))
	{
		this->args.fd = fd;
	}

	virtual int prepare()
	{
		this->prep_fdsync(this->args.fd);
		return 0;
	}
};

class __WFFilefdsyncTask : public WFFilefdsyncTask
{
public:
	__WFFilefdsyncTask(const std::string& filename, IOService *service,
					   fsync_callback_t&& cb) :
		WFFilefdsyncTask(-1, service, std::move(cb))
	{
		this->filename = filename;
	}

	virtual int prepare()
	{
		this->fd = open(this->filename.c_str(), O_WRONLY);
		if (fd < 0)
		{
			this->error = errno;
			return -1;
		}
		this->prep_fsync(this->args.fd);
		return 0;
	}

protected:
	virtual SubTask *done()
	{
		if (this->fd >= 0)
		{
			close(this->fd);
		}
		return WFFileTask::done();
	}

private:
	std::string filename;
	int fd;
};

/********FileIOTask*************/

WFFileIOTask *WFTaskFactory::create_pread_task(int fd,
											   void *buf,
											   size_t count,
											   off_t offset,
											   fio_callback_t callback)
{
	return new WFFilepreadTask(fd, buf, count, offset,
							   WFGlobal::get_io_service(),
							   std::move(callback));
}

WFFileIOTask *WFTaskFactory::create_pread_task(const std::string& filename,
											   void *buf,
											   size_t count,
											   off_t offset,
											   fio_callback_t callback)
{
	return new __WFFilepreadTask(filename, buf, count, offset,
								 WFGlobal::get_io_service(),
								 std::move(callback));
}

WFFileIOTask *WFTaskFactory::create_pwrite_task(int fd,
												const void *buf,
												size_t count,
												off_t offset,
												fio_callback_t callback)
{
	return new WFFilepwriteTask(fd, buf, count, offset,
								WFGlobal::get_io_service(),
								std::move(callback));
}

WFFileIOTask *WFTaskFactory::create_pwrite_task(const std::string& filename,
												const void *buf,
												size_t count,
												off_t offset,
												fio_callback_t callback)
{
	return new __WFFilepwriteTask(filename, buf, count, offset,
								  WFGlobal::get_io_service(),
								  std::move(callback));
}

WFFileVIOTask *WFTaskFactory::create_preadv_task(int fd,
												 const struct iovec *iovec,
												 int iovcnt,
												 off_t offset,
												 fvio_callback_t callback)
{
	return new WFFilepreadvTask(fd, iovec, iovcnt, offset,
								WFGlobal::get_io_service(),
								std::move(callback));
}

WFFileVIOTask *WFTaskFactory::create_preadv_task(const std::string& filename,
												 const struct iovec *iovec,
												 int iovcnt,
												 off_t offset,
												 fvio_callback_t callback)
{
	return new __WFFilepreadvTask(filename, iovec, iovcnt, offset,
								WFGlobal::get_io_service(),
								std::move(callback));
}

WFFileVIOTask *WFTaskFactory::create_pwritev_task(int fd,
												  const struct iovec *iovec,
												  int iovcnt,
												  off_t offset,
												  fvio_callback_t callback)
{
	return new WFFilepwritevTask(fd, iovec, iovcnt, offset,
								 WFGlobal::get_io_service(),
								 std::move(callback));
}

WFFileVIOTask *WFTaskFactory::create_pwritev_task(const std::string& filename,
												  const struct iovec *iovec,
												  int iovcnt,
												  off_t offset,
												  fvio_callback_t callback)
{
	return new __WFFilepwritevTask(filename, iovec, iovcnt, offset,
								 WFGlobal::get_io_service(),
								 std::move(callback));
}


WFFileSyncTask *WFTaskFactory::create_fsync_task(int fd,
												 fsync_callback_t callback)
{
	return new WFFilefsyncTask(fd,
							   WFGlobal::get_io_service(),
							   std::move(callback));
}

WFFileSyncTask *WFTaskFactory::create_fsync_task(const std::string& filename,
												 fsync_callback_t callback)
{
	return new __WFFilefsyncTask(filename,
								 WFGlobal::get_io_service(),
								 std::move(callback));
}

WFFileSyncTask *WFTaskFactory::create_fdsync_task(int fd,
												  fsync_callback_t callback)
{
	return new WFFilefdsyncTask(fd,
								WFGlobal::get_io_service(),
								std::move(callback));
}

WFFileSyncTask *WFTaskFactory::create_fdsync_task(const std::string& filename,
												  fsync_callback_t callback)
{
	return new __WFFilefdsyncTask(filename,
								  WFGlobal::get_io_service(),
								  std::move(callback));
}
