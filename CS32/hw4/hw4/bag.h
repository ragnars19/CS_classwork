// Bag.h

#ifndef BAG_INCLUDED
#define BAG_INCLUDED

#include <cstddef>

template<class ItemType>
class Bag
{
  public:
    Bag();               // Create an empty bag.
    bool empty() const;  // Return true if the bag is empty, otherwise false.

    int size() const;
      // Return the number of items in the bag.  For example, the size
      // of a bag containing "cumin", "cumin", "cumin", "turmeric" is 4.

    int uniqueSize() const;
      // Return the number of distinct items in the bag.  For example,
      // the uniqueSize of a bag containing "cumin", "cumin", "cumin",
      // "turmeric" is 2.

    bool insert(const ItemType& value);
      // Insert value into the bag.  Return true if the value was
      // actually inserted.  (For a linked list implementation, insert
      // always returns true.)

    int erase(const ItemType& value);
      // Remove one instance of value from the bag if present.
      // Return the number of instances removed, which will be 1 or 0.

    int eraseAll(const ItemType& value);
      // Remove all instances of value from the bag if present.
      // Return the number of instances removed.

    bool contains(const ItemType& value) const;
      // Return true if the value is in the bag, otherwise false.

    int count(const ItemType& value) const;
      // Return the number of instances of value in the bag.

    void swap(Bag& other);
      // Exchange the contents of this bag with the other one.

      // Iteration functions
    void start();                          // start an iteration
    void next();                           // advance to next item
    bool ended() const;                    // iteration has passed end
    const ItemType& currentValue() const;  // item at current position
    int currentCount() const;              // count of current item

      // Housekeeping functions
    ~Bag();
    Bag(const Bag& other);
    Bag& operator=(const Bag& rhs);

  private:
      // Representation:
      //   a circular doubly-linked list with a dummy node.
      //   m_head points to the dummy node.
      //   m_head->m_prev->m_next == m_head and m_head->m_next->m_prev == m_head
      //   m_size == 0  iff  m_head->m_next == m_head->m_prev == m_head
      //   If m_size > 0
      //       m_head->next points to the node at position 0.
      //       m_head->prev points to the node at position m_size-1.
      //   Nodes are in no particular order.

    struct Node
    {
        ItemType m_value;
        int      m_count;
        Node*    m_next;
        Node*    m_prev;
    };

    Node* m_head;
    int   m_uniqueSize;
    int   m_size;
    Node* m_current;      // pointer to current node during iteration

    Node* find(const ItemType& value) const;
      // Return pointer to Node whose m_value == value if there is one,
      // else m_head

    int doErase(const ItemType& value, bool all);
      // Remove one or all instances of value from the bag if present,
      // depending on the second parameter.  Return the number of instances
      // removed.
};

// Declarations of non-member functions
template<class ItemType>
void combine(Bag<ItemType>& b1, Bag<ItemType>& b2, Bag<ItemType>& result); 
      // If a value occurs n1 times in b1 and n2 times in b2, then
      // it will occur n1+n2 times in result upon return from this function.

template<class ItemType>
void subtract(Bag<ItemType>& b1, Bag<ItemType>& b2, Bag<ItemType>& result); 
      // If a value occurs n1 times in b1 and n2 times in b2, then
      // it will occur n1-n2 times in result upon return from this function
      // if n1 >= n2.  If n1 <= n2, it will not occur in result.

// Inline implementations

template<class ItemType>
inline int Bag<ItemType>::size() const
{
    return m_size;
}

template<class ItemType>
inline int Bag<ItemType>::uniqueSize() const
{
    return m_uniqueSize;
}

template<class ItemType>
inline bool Bag<ItemType>::empty() const
{
    return size() == 0;
}

template<class ItemType>
inline int Bag<ItemType>::erase(const ItemType& value)
{
    return doErase(value, false);
}

template<class ItemType>
inline int Bag<ItemType>::eraseAll(const ItemType& value)
{
    return doErase(value, true);
}

template<class ItemType>
inline bool Bag<ItemType>::contains(const ItemType& value) const
{
    return find(value) != m_head;
}

template<class ItemType>
inline void Bag<ItemType>::start()
{
    m_current = m_head->m_next;
}

template<class ItemType>
inline void Bag<ItemType>::next()
{
    m_current = m_current->m_next;
}

template<class ItemType>
inline bool Bag<ItemType>::ended() const
{
    return m_current == m_head;
}

template<class ItemType>
inline const ItemType& Bag<ItemType>::currentValue() const
{
    return m_current->m_value;
}

template<class ItemType>
inline int Bag<ItemType>::currentCount() const
{
    return m_current->m_count;
}

void exchange(int& a, int& b)
{
    int t = a;
    a = b;
    b = t;
}

// To help clients detect any reliance on undefined iteration behavior,
// whenever we can, we invalidate access through the m_current pointer
// by setting it to NULL whenever the state of the iteration is not defined.

template<class ItemType>
Bag<ItemType>::Bag()
 : m_uniqueSize(0), m_size(0), m_current(NULL)
{
      // create dummy node
    m_head = new Node;
    m_head->m_next = m_head;
    m_head->m_prev = m_head;
}

template<class ItemType>
Bag<ItemType>::~Bag()
{
      // Delete the m_uniqueSize non-dummy nodes plus the dummy node

    for (Node* p = m_head->m_prev ; m_uniqueSize >= 0; m_uniqueSize--)
    {
	Node* toBeDeleted = p;
	p = p->m_prev;
	delete toBeDeleted;
    }
}

template<class ItemType>
Bag<ItemType>::Bag(const Bag& other)
 : m_uniqueSize(other.m_uniqueSize), m_size(other.m_size), m_current(NULL)
{
      // Create dummy node; don't initialize its m_next

    m_head = new Node;
    m_head->m_prev = m_head;

      // Copy each node from the other list; each iteration will set the
      // m_next of the previous node copied

    for (Node* p = other.m_head->m_next ; p != other.m_head; p = p->m_next)
    {
	  // Create a copy of the node p points to
	Node* pnew = new Node;
	pnew->m_value = p->m_value;
	pnew->m_count = p->m_count;
	
	  // Connect the m_prev pointers
	pnew->m_prev = m_head->m_prev;
	m_head->m_prev = pnew;

	  // Connect the previous Node's m_next 
	pnew->m_prev->m_next = pnew;
    }

      // Connect the last Node's m_next 
    m_head->m_prev->m_next = m_head;
}

template<class ItemType>
Bag<ItemType>& Bag<ItemType>::operator=(const Bag& rhs)
{
    if (this != &rhs)
    {
	Bag temp(rhs);
	swap(temp);
    }
    return *this;
}

template<class ItemType>
bool Bag<ItemType>::insert(const ItemType& value)
{
    Node* p = find(value);

    if (p != m_head)  // found
        p->m_count++;
    else
    {
	  // Create a new node
	p = new Node;
	p->m_value = value;
	p->m_count = 1;

	  // Insert new item at tail of list (arbitrary choice of position)
	  //     Connect it to tail
	p->m_prev = m_head->m_prev;
	p->m_prev->m_next = p;

	  //     Connect it to dummy node
	p->m_next = m_head;
	m_head->m_prev = p;

	m_uniqueSize++;
    }

    m_size++;
    m_current = NULL;  // invalidate iteration -- list changed
    return true;
}

template<class ItemType>
int Bag<ItemType>::count(const ItemType& value) const
{
    Node* p = find(value);
    return p == m_head ? 0 : p->m_count;
}

template<class ItemType>
void Bag<ItemType>::swap(Bag& other)
{
      // swap head pointers
    Node* temp = m_head;
    m_head = other.m_head;
    other.m_head = temp;

      // swap uniqueSize and size
    exchange(m_uniqueSize, other.m_uniqueSize);
    exchange(m_size, other.m_size);

      // iterator state after swap is undefined.  Invalidate iteration.
    m_current = NULL;
    other.m_current = NULL;
}

template<class ItemType>
typename Bag<ItemType>::Node* Bag<ItemType>::find(const ItemType& value) const
{
      // Do a linear search through the list

    Node* p;
    for (p = m_head->m_next; p != m_head && p->m_value != value; p = p->m_next)
	;
    return p;
}

template<class ItemType>
int Bag<ItemType>::doErase(const ItemType& value, bool all)
{
    Node* p = find(value);

    if (p == m_head)  // not found
	return 0;

    int nErased = (all ? p->m_count : 1);  // number to erase

      // If erasing one, and there are more than one, just decrement;
      // otherwise, we're erasing all, or erasing one whose count is 1,
      // so unlink the Node from the list and destroy it

    if (!all  &&  p->m_count > 1)
	p->m_count--;
    else
    {
        p->m_prev->m_next = p->m_next;
        p->m_next->m_prev = p->m_prev;
        delete p;

        m_uniqueSize--;
    }

    m_size -= nErased;
    m_current = NULL;  // invalidate iteration -- list changed
    return nErased;
}

template<class ItemType>
void combine(Bag<ItemType>& b1, Bag<ItemType>& b2, Bag<ItemType>& result)
{
      // Guard against the case that result is an alias for b1 or b2
      // (i.e., that result is a reference to the same bag that b1 or b2
      // refers to) by building the answer in a local variable res.  When
      // done, swap res with result; the old value of result (now in res) will
      // be destroyed when res is destroyed.

    Bag<ItemType> res(b1);
    for (b2.start(); !b2.ended(); b2.next())
    {
	for (int k = 0; k < b2.currentCount(); k++)
	    res.insert(b2.currentValue());
    }
    result.swap(res);
}

template<class ItemType>
void subtract(Bag<ItemType>& b1, Bag<ItemType>& b2, Bag<ItemType>& result)
{
      // Guard against the case that result is an alias for b1 or b2
      // by building the answer in a local variable res.  When done, swap res
      // with result; the old value of result (now in res) will be destroyed
      // when res is destroyed.

    Bag<ItemType> res;
    for (b1.start(); !b1.ended(); b1.next())
    {
        int n = b1.currentCount() - b2.count(b1.currentValue());
	for (int k = 0; k < n; k++)
	    res.insert(b1.currentValue());
    }
    result.swap(res);
}

#endif // BAG_INCLUDED