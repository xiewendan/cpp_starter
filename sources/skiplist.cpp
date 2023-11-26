#include <assert.h>
#include <iostream>
#include <vector>

const int MAX_LEVEL = 5;

template <typename K, typename V>
class Node {
 public:
  Node(K nKey, V nValue, int nLevel)
      : m_nKey(nKey), m_nValue(nValue), m_vecNext(nLevel + 1, nullptr) {
    assert(nLevel >= 0);
  }

  ~Node() {}

  int GetKey() const { return m_nKey; }

  int GetValue() const { return m_nValue; }

  Node<K, V> *GetNextNode(int nLevel) {
    int nMaxLevel = GetMaxLevel();
    assert(nLevel <= nMaxLevel);
    return m_vecNext[nLevel];
  }

  int GetMaxLevel() { return m_vecNext.size() - 1; }

  void Resize(int nLevel) { m_vecNext.resize(nLevel + 1, nullptr); }

  void SetNextNode(Node<K, V> *pNextNode, int nLevel) {
    int nMaxLevel = GetMaxLevel();
    assert(nLevel <= nMaxLevel);
    m_vecNext[nLevel] = pNextNode;
  }

 private:
  K m_nKey;
  V m_nValue;
  std::vector<Node<K, V> *> m_vecNext;
};

template <typename K, typename V>
class SkipList {
 public:
  SkipList() : m_nLevel(0) { m_pHeadObj = new Node<K, V>(0, 0, m_nLevel); }
  ~SkipList() {
    Node<K, V> *pCurNode = m_pHeadObj;
    while (pCurNode) {
      Node<K, V> *pTemp = pCurNode->GetNextNode(0);
      delete pCurNode;
      pCurNode = pTemp;
    }
    m_pHeadObj = nullptr;
  }

  bool Contain(K nKey) {
    bool bFind = false;
    _Search(nKey, bFind);

    std::cout << "search value:" << nKey << ", bFind:" << bFind << std::endl;
    return bFind;
  }

  V Search(K nKey) {
    bool bFind = false;
    std::vector<Node<K, V> *> vecPreNode = _Search(nKey, bFind);

    assert(bFind);

    Node<K, V> *pPreNode = vecPreNode[0];
    Node<K, V> *pCurNode = pPreNode->GetNextNode(0);
    V nValue = pCurNode->GetValue();
    std::cout << "search key:" << nKey << ", value:" << nValue << std::endl;

    return nValue;
  }

  bool Insert(K nKey, V nValue) {
    bool bFind = false;
    std::vector<Node<K, V> *> vecPreNode = _Search(nKey, bFind);
    if (bFind) {
      std::cout << "insert failed, already exist:" << nKey << std::endl;
      return false;
    }
    Node<K, V> *pNewNode = _CreateNode(nKey, nValue);
    _Insert(pNewNode, vecPreNode);
    std::cout << "insert succeed:" << nKey << std::endl;
    return true;
  }

  bool Remove(K nKey) {
    bool bFind = false;
    std::vector<Node<K, V> *> vecPreNode = _Search(nKey, bFind);
    if (!bFind) {
      std::cout << "remove failed, not exist:" << nKey << std::endl;
      return false;
    }
    Node<K, V> *pDelNode = vecPreNode[0]->GetNextNode(0);
    _Remove(pDelNode, vecPreNode);
    std::cout << "remove succeed:" << nKey << std::endl;
    return true;
  }
  void Display() {
    for (int i = m_nLevel; i >= 0; i--) {
      std::cout << "level : " << i;
      Node<K, V> *pCurrent = m_pHeadObj->GetNextNode(i);
      while (pCurrent) {
        std::cout << " " << pCurrent->GetKey() << ":" << pCurrent->GetValue();

        pCurrent = pCurrent->GetNextNode(i);
      }
      std::cout << std::endl;
    }
  }

 private:
  std::vector<Node<K, V> *> _Search(K nKey, bool &bFind) {
    std::vector<Node<K, V> *> vecPreNode(m_nLevel + 1);
    Node<K, V> *pPreNode = m_pHeadObj;
    for (int i = m_nLevel; i >= 0; i--) {
      while (pPreNode->GetNextNode(i) &&
             pPreNode->GetNextNode(i)->GetKey() < nKey) {
        pPreNode = pPreNode->GetNextNode(i);
      }
      vecPreNode[i] = pPreNode;
    }

    Node<K, V> *pCurNode = pPreNode->GetNextNode(0);
    bFind = pCurNode && pCurNode->GetKey() == nKey;
    return vecPreNode;
  }
  Node<K, V> *_CreateNode(K nKey, V nValue) {
    int nLevel = 0;
    while (nLevel < MAX_LEVEL && (rand() % 2) == 1) {
      nLevel += 1;
    }

    return new Node<K, V>(nKey, nValue, nLevel);
  }
  void _Insert(Node<K, V> *pNewNode,
               const std::vector<Node<K, V> *> &vecPreNode) {
    int nNewNodeMaxLevel = pNewNode->GetMaxLevel();
    int nPreNodeMaxLevel = vecPreNode.size() - 1;
    int nMinLevel = nNewNodeMaxLevel > nPreNodeMaxLevel ? nPreNodeMaxLevel
                                                        : nNewNodeMaxLevel;
    for (int i = nMinLevel; i >= 0; i--) {
      Node<K, V> *pPreNode = vecPreNode[i];
      pNewNode->SetNextNode(pPreNode->GetNextNode(i), i);
      pPreNode->SetNextNode(pNewNode, i);
    }

    if (nNewNodeMaxLevel > m_nLevel) {
      m_pHeadObj->Resize(nNewNodeMaxLevel);
      for (int i = nNewNodeMaxLevel; i > m_nLevel; i--) {
        m_pHeadObj->SetNextNode(pNewNode, i);
      }

      m_nLevel = nNewNodeMaxLevel;
    }
  }
  void _Remove(Node<K, V> *pDelNode,
               const std::vector<Node<K, V> *> &vecPreNode) {
    int nDelNodeMaxLevel = pDelNode->GetMaxLevel();
    int nPreNodeMaxLevel = vecPreNode.size() - 1;
    int nMinLevel = nDelNodeMaxLevel > nPreNodeMaxLevel ? nPreNodeMaxLevel
                                                        : nDelNodeMaxLevel;

    for (int i = nMinLevel; i >= 0; i--) {
      Node<K, V> *pPreNode = vecPreNode[i];
      pPreNode->SetNextNode(pDelNode->GetNextNode(i), i);
    }

    delete pDelNode;

    while (m_nLevel > 0 && m_pHeadObj->GetNextNode(m_nLevel) == nullptr) {
      m_nLevel -= 1;
    }
  }

 private:
  Node<K, V> *m_pHeadObj;
  int m_nLevel;
};

int main() {
  std::cout << "hello world" << std::endl;
  SkipList<int, int> SkipListObj;
  SkipListObj.Insert(20, 200);
  SkipListObj.Insert(19, 190);
  SkipListObj.Insert(18, 180);
  SkipListObj.Insert(17, 170);
  SkipListObj.Insert(11, 110);
  SkipListObj.Insert(10, 100);
  SkipListObj.Insert(9, 90);
  SkipListObj.Insert(4, 40);
  SkipListObj.Insert(3, 30);
  SkipListObj.Insert(2, 20);
  SkipListObj.Insert(1, 10);
  SkipListObj.Insert(0, 1);
  SkipListObj.Display();

  if (SkipListObj.Contain(0)) {
    SkipListObj.Search(0);
  }
  if (SkipListObj.Contain(10)) {
    SkipListObj.Search(10);
  }
  if (SkipListObj.Contain(20)) {
    SkipListObj.Search(20);
  }
  if (SkipListObj.Contain(-2)) {
    SkipListObj.Search(-2);
  }
  if (SkipListObj.Contain(5)) {
    SkipListObj.Search(5);
  }
  if (SkipListObj.Contain(15)) {
    SkipListObj.Search(15);
  }

  if (SkipListObj.Contain(25)) {
    SkipListObj.Search(25);
  }

  if (SkipListObj.Contain(5)) {
    SkipListObj.Remove(5);
  }
  if (SkipListObj.Contain(10)) {
    SkipListObj.Remove(10);
  }
  SkipListObj.Display();
  if (SkipListObj.Contain(0)) {
    SkipListObj.Remove(0);
  }
  SkipListObj.Display();
  if (SkipListObj.Contain(20)) {
    SkipListObj.Remove(20);
  }

  SkipListObj.Display();

  return 0;
}
