#ifndef _TEST_SUPPORT_H_
#define _TEST_SUPPORT_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <exception>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <utime.h>

namespace Test {

using namespace std;

static void
touchFile(const char *filename, time_t timestamp = (time_t) - 1) {
	FILE *f = fopen(filename, "a");
	if (f != NULL) {
		fclose(f);
	} else {
		int e = errno;
		cerr << "Cannot touch file '" << filename << "': " <<
			strerror(e) <<" (" << e << ")" << endl;
		throw exception();
	}
	
	if (timestamp != (time_t) -1) {
		struct utimbuf times;
		times.actime = timestamp;
		times.modtime = timestamp;
		utime(filename, &times);
	}
}

/**
 * Class which creates a temporary directory of the given name, and deletes
 * it upon destruction.
 */
class TempDir {
private:
	string name;
public:
	TempDir(const string &name) {
		this->name = name;
		if (mkdir(name.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
			int e = errno;
			cerr << "Cannot create directory '" << name << "': " <<
				strerror(e) <<" (" << e << ")" << endl;
			throw exception();
		}
	}
	
	~TempDir() {
		string command("rm -rf \"");
		command.append(name);
		command.append("\"");
		system(command.c_str());
	}
};

/**
 * Creates a temporary copy of the given directory. This copy is deleted
 * upon object destruction.
 */
class TempDirCopy {
private:
	string dir;
	
	void rm_rf(const string &dir) {
		string command("rm -rf \"");
		command.append(dir);
		command.append("\"");
		system(command.c_str());
	}
public:
	TempDirCopy(const string &source, const string &dest) {
		dir = dest;
		rm_rf(dest);
		
		char command[1024];
		snprintf(command, sizeof(command), "cp -pR \"%s\" \"%s\"",
			source.c_str(), dest.c_str());
		system(command);
	}
	
	~TempDirCopy() {
		rm_rf(dir);
	}
};

/**
 * Class which deletes the given file upon destruction.
 */
class DeleteFileEventually {
private:
	string filename;
public:
	DeleteFileEventually(const string &filename) {
		this->filename = filename;
	}
	
	~DeleteFileEventually() {
		unlink(filename.c_str());
	}
};

} // namespace Test

#endif /* _TEST_SUPPORT_H_ */
