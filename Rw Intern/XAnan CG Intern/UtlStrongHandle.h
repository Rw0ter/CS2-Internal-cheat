#pragma once
template <class T>
struct ResourceBinding {
    T* data;
    unsigned long long unk;
    unsigned long long unk2;
    volatile unsigned int* ref_count;
};

template <class T>
class CStrongHandle {
public:
    bool operator!() {
        return !IsValid();
    }

    operator T*() const {
        return IsValid() ? reinterpret_cast<T*>(Binding->data) : nullptr;
    }

    T* operator->() const {
        return IsValid() ? reinterpret_cast<T*>(Binding->data) : nullptr;
    }

    [[nodiscard]] bool IsValid() const { return Binding->data != nullptr; }
    const ResourceBinding<T>* Binding;
};