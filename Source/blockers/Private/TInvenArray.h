#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Misc/Optional.h"

template<typename ElementType>
USTRUCT(BlueprintType)
class TInvenArray
{
public:
    // ��� �߰�
    void Add(int32 Index, const ElementType& Element)
    {
        //EnsureCapacity(Index);
        Elements[Index] = Element;
    }

    // ��� ���� (const ����)
    TOptional<ElementType> Get(int32 Index) const
    {
        if (Index < Elements.Num())
        {
            return Elements[Index];
        }
        return TOptional<ElementType>();
    }

    // ��� ���� (non-const ����)
    TOptional<ElementType>& Get(int32 Index)
    {
        if (Index < Elements.Num())
        {
            return Elements[Index];
        }
        static TOptional<ElementType> EmptyValue;
        return EmptyValue;
    }

    // ��� ����
    void Remove(int32 Index)
    {
        if (Index < Elements.Num())
        {
            Elements[Index].Reset();
        }
    }

    // �迭 ũ�� ��ȯ
    int32 Num() const
    {
        return Elements.Num();
    }

private:
    TArray<TOptional<ElementType>> Elements;

    // �ʿ��� ũ����� �迭�� Ȯ��
    void EnsureCapacity(int32 Index)
    {
        if (Index >= Elements.Num())
        {
            Elements.SetNum(Index + 1);
        }
    }
};
