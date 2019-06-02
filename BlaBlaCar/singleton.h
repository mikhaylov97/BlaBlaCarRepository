#ifndef SINGLETON_H
#define SINGLETON_H
template < typename T >
class Singleton {
  public:
    static T & get_instance() {
      static MemGuard g; // clean up on program end
      if (!m_instance) {
        m_instance = new T();
      }
      return * m_instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton) = delete;

  protected:
    Singleton() { };
    virtual ~Singleton() { }

  private:
    static T * m_instance;

    class MemGuard {
      public:
        ~MemGuard() {
          delete m_instance;
          m_instance = nullptr;
        }
    };
};
#endif // SINGLETON_H
