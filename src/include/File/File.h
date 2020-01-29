#ifndef FILE_H
#define FILE_H
//Clase solo para sistemas Unix-like(o que usen POSIX),
//pero hare una adaptacion para Windows con sus mismas APIs
#ifdef __linux__//solo lo he probado en linux, por eso lo pongo
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <mntent.h>
#include <sys/stat.h>
#include <dirent.h>
#include "../String/String.h"
#include "../ArrayList/ArrayList.h"
#include "../Exception/SecurityException.h"

#ifndef NULL
#define NULL 0
#endif
class File
{
private:
	ArrayList<String> dirStructure;
	String mPathName;
	struct stat mStatFile;
	bool mExist;
	bool mAccess;
	static String DOT;
	static String DOTDOT;
	static const char* INFO_FILE;
	//=======Trabajando en utilerias como esta======
	void zeroMemory(void *mem, int len)
	{
		char *m = (char*)mem;
		for (int i = 0; i<len;i++)
		{
			m[i] = 0;
		}
	}
	//=======
protected:
	ArrayList<String> toAbsolutePath(String *path)
	{
		if (path->charAt(0)=='/')
		{
			int lastIndex = 1;
			ArrayList<String>folders;
			folders.add("/");
			int count = 1;
			for (int i = 1; i < path->length();i++)
			{
				if (path->charAt(i) == '/')
				{
					String folder = path->substring(lastIndex, i - lastIndex + 1);
					lastIndex = i+1;
					if (folder == "../")
					{
						if (count > 1)
							count--;
					}
					else if (!folder.equals("./"))
					{
						folders.add(count, folder);
						count++;
					}
				}
			}

			if (lastIndex < path->length())
			{
				folders.add(count++, path->substring(lastIndex));
			}
			return folders.subArrayList(0, count);

		}else//se trata de una ruta relativa
		{
			char workDir[PATH_MAX];
			getcwd(workDir, PATH_MAX);
			String ruta = workDir;
			if (ruta.charAt(ruta.length()-1)!='/') ruta+= "/";
			ruta += *path;
			return toAbsolutePath(&ruta);
		}
	}
public:
	static int FILES_AND_FOLDERS;
	static int ONLY_FILES;
	static int ONLY_FOLDERS;
	virtual ~File(){
	}
	File(String pathName):
		dirStructure(),
		mPathName(pathName),
		mAccess(true)
	{
		zeroMemory(&mStatFile, sizeof(mStatFile));
		if (stat(pathName, &mStatFile) == -1)
		{
			mExist = (errno != ENOENT);
			mAccess = ((errno & EACCES) != EACCES);
		}
		dirStructure = toAbsolutePath(&pathName);
	}
	static ArrayList<String> listRoots();
	static bool setWorkDir(String);
	static String getWorkDir();
	virtual long int length()
	{
		return mStatFile.st_size;
	}

	virtual long int lastModified()
	{
		return mStatFile.st_mtim.tv_nsec;
	}
	virtual long int lastAccess()
	{
		return mStatFile.st_atim.tv_nsec;
	}
	virtual long int lastChange()
	{
		return mStatFile.st_ctime;
	}
	virtual bool exist()
	{
		return mExist;
	}
	virtual unsigned int getPermissions()
	{
		return mStatFile.st_mode;
	}
	virtual bool setPermissions(unsigned int permissions)
	{
		//vea: http://linux.die.net/man/2/chmod
		return	~chmod(getAbsolutePath(), permissions);
	}
	virtual int getHardLinkTo()
	{
		return mStatFile.st_nlink;
	}
	virtual bool isSymbolic()//Si es una liga dura
	{
		return (S_IFLNK & mStatFile.st_mode) == S_IFLNK;
	}
	virtual bool isDirectory()
	{
		//return S_ISDIR(mStatFile.st_mode);
		return (S_IFDIR & mStatFile.st_mode) == S_IFDIR;
	}

	virtual bool isFile()
	{
		//return S_ISREG(mStatFile.st_mode);
		return (S_IFREG & mStatFile.st_mode) == S_IFREG;
	}
	virtual bool canRead()
	{
		if (mAccess)
			return (S_IRUSR & mStatFile.st_mode) == S_IRUSR;

		return false;
	}
	virtual bool canWrite()
	{
		if (mAccess)
			return (S_IWUSR & mStatFile.st_mode) == S_IWUSR;
		return false;
	}
	virtual ArrayList<String> list(int arg = FILES_AND_FOLDERS)
	{
		ArrayList<String> dirs;
		if (isDirectory())
		{
			DIR* srcDir = opendir(mPathName);
			struct dirent* dent;
			if (srcDir == NULL)
			{
				switch(errno)
				{
				case EACCES:
					throw SecurityException("No tiene permisos para acceder a la carpeta");
				case ELOOP:
					throw Exception("Muchas ligas simbolicas encontradas");
				case ENAMETOOLONG:
					throw Exception("The length of the path argument exceeds PATH_MAX, or a pathname component is longer than NAME_MAX");
				case ENOENT:
					throw Exception("No existe la carpeta");
				case ENOTDIR:
					throw Exception("El pathname no es carpeta");
					//no deberia pasar esto
				default:
					throw Exception("No se puede leer desde la carpeta");
				}
				//strerror en string.h, es mas especifico
			}

			while((dent = readdir(srcDir)) !=  NULL)
			{
				struct stat st;
				if (!DOT.equals(dent->d_name) && !DOTDOT.equals(dent->d_name))
				{
					if (fstatat(dirfd(srcDir), dent->d_name, &st,0) < 0)
					{
						continue;
					}
					if (arg == FILES_AND_FOLDERS)
					{
						dirs.add(dent->d_name);
					}
					else if (arg == ONLY_FILES)
					{
						if ((st.st_mode & S_IFREG) == S_IFREG)
						{
							dirs.add(dent->d_name);
						}
					}
					else
					{
						if ((st.st_mode & S_IFDIR) ==S_IFDIR)
						{
							dirs.add(dent->d_name);
						}
					}
				}
			}
			closedir(srcDir);
		}
		return dirs;
	}
	virtual bool mkdir(bool nothrow = true)
	{//http://linux.die.net/man/2/mkdir
		mode_t modo;
		modo = S_IRWXU | S_IRWXG | S_IRWXO;//pueden cambiar los valores, pero estos son por default en linux
		if (nothrow)
		{
			return ~::mkdir(getAbsolutePath(), modo);
		}
		else
		{
			if (::mkdir(getAbsolutePath(), modo) == -1)
			{
				switch(errno)
				{
				case EACCES:
					throw SecurityException("The parent directory does not allow write permission to the process, or one of the directories in pathname did not allow search permission");
				case EEXIST:
					throw Exception("Pathname already exists (not necessarily as a directory)");
				case EROFS:
					throw Exception("pathname refers to a file on a read-only file system.");
				case EPERM:
					throw Exception("The file system containing pathname does not support the creation of directories");
				case ENOSPC:
					throw Exception("The new directory cannot be created because the user's disk quota is exhausted");
				case ENOMEM:
					throw Exception("Insufficient kernel memory was available");
				case ENOENT:
					throw Exception("A directory component in pathname does not exist or is a dangling symbolic link.");
				default:
					throw Exception(String("Excepcion no manejada: ") + errno + "\nVea: //http://linux.die.net/man/2/mkdir");
				}
			}
		}
		return true;
	}
	virtual String getRelativePath()
	{
		char workDir[PATH_MAX];
		getcwd(workDir, PATH_MAX);
		String ruta = workDir;
		String ret;
		if (ruta.charAt(ruta.length()-1)!='/') ruta+= "/";
		ArrayList<String> workStructure = toAbsolutePath(&ruta);
		int index = dirStructure.size() - 1;
		for (int i = 0; i < dirStructure.size();i++)//bucle para saber si debemos hacer "../"
		{
			if (workStructure.size()>i){
				if (dirStructure[i] != workStructure[i]){
					index = i;
					break;
				}
			}
			else{
				index = i;
				break;
			}
		}

		if (index == 0)index++;//para no incluir el "/"
		for (int i = workStructure.size();i>index;i--)
		{
			ret += "../";
		}
		for (;index < dirStructure.size();index++)
		{
			ret+=dirStructure[index];
		}
		return ret;
	}
	virtual String getParent()
	{
		if (dirStructure.size()>1)
		{
			return dirStructure[dirStructure.size()-2];
		}
		return dirStructure[0];
	}
	virtual String getName()
	{
		return dirStructure[dirStructure.size()-1];
	}
	virtual String getAbsolutePath()
	{
		String ret;
		for (int  i = 0; i <  dirStructure.size();i++)
		{
			ret+= dirStructure[i];
		}
		return ret;
	}

	virtual bool remove()
	{
		int flag = isDirectory()?AT_REMOVEDIR: 0;
		return ~unlinkat(0, getAbsolutePath(), flag);
	}

};
String File::DOT = ".";
String File::DOTDOT = "..";
const char* File::INFO_FILE = "/etc/mtab";
int File::FILES_AND_FOLDERS = 0;
int File::ONLY_FILES = 1;
int File::ONLY_FOLDERS = 2;
String File::getWorkDir()
{
	char workDir[PATH_MAX];
	getcwd(workDir, PATH_MAX);
	return workDir;
}
bool File::setWorkDir(String dir)
{
	return ~chdir(dir);
}
ArrayList<String> File::listRoots()
{
	ArrayList<String> list;
	FILE *f = setmntent(INFO_FILE, "r");
	if (f != NULL)
	{
		struct mntent *prop;
		while((prop = getmntent(f))!= NULL)
		{
			String fsname = prop->mnt_fsname;
			if (fsname != "none")
			{
				String prefix = fsname.substring(0,6);
				if (prefix == "/dev/s" || prefix == "/dev/h")
					list.add(prop->mnt_dir);//donde esta montado
			}
		}
		endmntent(f);
	}
	return list;
}
#endif
#endif

