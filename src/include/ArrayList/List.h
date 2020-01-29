#ifndef LIST_H
#define LIST_H
template <class T>
class List
{
    public:
        virtual bool add(T element) = 0;
        virtual bool add(int index, T element) = 0;
        virtual bool addAll(List<T>*c) = 0;
        virtual bool addAll(int index, List<T>*c) = 0;
        virtual void clear() = 0;
        virtual bool isEmpty() = 0;
        virtual T remove(int index) = 0;
        virtual bool removeAll(List <T>*c) = 0;
        virtual bool retainAll(List <T>*c) = 0;
        virtual int size() = 0;
        virtual T* toArray() = 0;
        virtual T get(int index) = 0;
        virtual T set(int index, T element) = 0;
        virtual int indexOf(T element) = 0;
        virtual int lastIndexOf(T element) = 0;
        virtual List<T> *subList(int fromIndex, int toIndex) = 0;
        virtual ~List(){}
};
template <class T>
class List<T*>
{
    public:
        virtual bool add(T *element) = 0;
        virtual bool add(int index, T *element) = 0;
        virtual bool addAll(List<T*>*c) = 0;
        virtual bool addAll(int index, List<T*> *c) = 0;
        virtual void clear() = 0;
        virtual bool contains(T *element) = 0;
        virtual bool containsAll(List<T*>*c) = 0;
        virtual bool isEmpty() = 0;
        virtual T*   remove(int index) = 0;
        virtual bool removeAll(List <T*>*c) = 0;
        virtual bool retainAll(List <T*>*c) = 0;
        virtual int  size() = 0;
        virtual T**  toArray() = 0;
        virtual T*   get(int index) = 0;
        virtual T*   set(int index, T *element) = 0;
        virtual int  indexOf(T *element) = 0;
        virtual int  lastIndexOf(T *element) = 0;
        virtual List<T*> *subList(int fromIndex, int toIndex) = 0;
        virtual ~List(){}
};

#endif
