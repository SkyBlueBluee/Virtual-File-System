#pragma once
// Many different ways to handle the file load/read/write part.

// Things to consider:
// - Mounting:
//   When mounting, should the files be recursively read and loaded? Its super expensive to do so. 
//   The sub-directories, however, will definitely be recursively loaded as well. 
//   Furthermore, the mount should have an option where users can choose if they would like events to be sent when a file/dir has changed.
//   Perhaps for editor would be ok... But on release we should generate a list of files that we KNOW will be used. 
//   Other files can be opened during run time. If its just reading, should be quite fast. 
//   When a situation where 2 mounts with diff paths but with the same alias occurs, the system should search the last mounted
//    dir before searching the first one. This is to allow more flexibility when it comes to patching.
//   Example of the above:
//    If I want to change a texture but I'm not sure if the new one will work, simply mount a second folder with the same alias 
//    and the system will use the new texture. If you feel like its not to your liking, you can simply delete the texture and the 
//    system will use the old texture. 

// - Writing to file:
//   Could have a function called file_write which takes in a big buffer and writes it to that file. 
//   Alternatively, could leverage C++ I/O to open/create files to write to. Should help a lot with serialization.

// - Loading files:
//   Perhaps for loading, we can have a function that returns a file handle and the user should check that file handle for errors.
//   Question is, should the handle load the data when the user calls the load function? Allows the users to have more control over whent he data is generated.
//   Or allow the user to construct the file handle using the path (with the proper alias)... Probably this way is better.
//   Also, should a file be created in the case where the file cannot be found? (similar to C++)
//   Need to account for the case where the user does not know which sub directory the file is located in. Probably not a priority. 

// - Reading files:
//   Some stuff to consider is to allow users to perform an action on every single file of a specific type from a specified dir.
//   Eg: I want to load all the png files. vfs->for_each_file<function>("/resource/textures", ....). Not a priority. 
//   It is important for file reading to be optimized. Can make use of RAII where we create a temp file handle that will deallocatea
//   memory when it goes out of scope. 

// - Sub-directories
//   Should allow users to search for a specific sub-dir within a specified dir. Not sure if this is even needed but just putting it down for now.
//   

struct FileHandle
{
	
	size_t size = 0;
	char* data = nullptr;
};

class VFS
{
public:

	enum VFS_ERROR
	{
		VFS_OK = 0,
		VFS_ERROR_INVALID_PATH = -1,
		VFS_ERROR_INVALID_MOUNT = -2,
		VFS_ERROR_FAILED_TO_READ_FILE = -3,
		VFS_ERROR_FILE_NOT_FOUND = -4,
		VFS_ERROR_DIR_NOT_FOUND = -5,
		VFS_ERROR_INVALID_PARAMETER = -6,
		VFS_ERROR_INVALID_FILE_HANDLE = -7
	};


	VFS();
	VFS_ERROR mount(const char* path, const char* alias_path);
	VFS_ERROR dismount(const char* alias_path);
	VFS_ERROR dismount_all();

	// Use the alias path. No allocation is done. Should create the file if the file cannot be found?
	VFS_ERROR file_load(const char* path, FileHandle& handle);
	
	void Init();
	void Update(float dt);
	void Shutdown();
private:

};