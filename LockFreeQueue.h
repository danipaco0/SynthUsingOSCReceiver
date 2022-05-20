template<typename T>
class LockFreeQueue
{
public:
    LockFreeQueue()
    {
        first = new Node(T()); // Dummy seperator.

        last.set(first);
        divider.set(first);
    }

    ~LockFreeQueue()
    {
        while (first != nullptr)
        {
            Node* toDel = first;
            first = toDel->next;
            delete toDel;
        }
    }

    /**
     * Add an item to the queue. Obviously, should only be called from producer's thread.
     */
    void push(const T& t)
    {
        last.get()->next = new Node(t);

        last.set(last.get()->next);

        while (first != divider.get()) { // trim unused nodes
            Node* tmp = first;
            first = first->next;
            delete tmp;
        }
    }

    /**
     * Consume an item in the queue. Returns false if no items left to consume.
     */
    <T> pop()
    {
        Node* div = divider.get();

        while (div && !head.compare_exchange_weak(div, div->next)) 
        {
            div = divider.load();
        }
        return div ? div->value : <T>();
    }

private:
    struct Node
    {
        Node(const T& val) : value(val), next(nullptr) {}

        T value;
        Node* next;
    };

    Node* first;
    Atomic<Node*> divider, last;
};
