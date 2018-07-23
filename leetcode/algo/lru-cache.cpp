// https://leetcode.com/problems/lru-cache/description/
// 52ms


class LRUCache {
    struct BitMap256 {
        uint32_t m_bitmap[8]; // 32bit * 8

        BitMap256() {
            memset(m_bitmap, 0, sizeof(m_bitmap));
        }

        inline void MarkBit(unsigned char idx) {
            m_bitmap[idx/32] |= (1 << (idx % 32));
        }

        inline void EraseBit(unsigned char idx) {
            m_bitmap[idx/32] &= ~(1 << (idx % 32));
        }

        inline bool BitExist(unsigned char idx) {
            return m_bitmap[idx/32] & (1 << (idx % 32));
        }

        inline bool Empty() {
            return (m_bitmap[0] | m_bitmap[1] | m_bitmap[2] | m_bitmap[3] |
                    m_bitmap[4] | m_bitmap[5] | m_bitmap[6] | m_bitmap[7]) == 0;
        }
    };


    struct CacheEntry {
        int m_key;
        int m_value;
        CacheEntry *m_prev;
        CacheEntry *m_next;
    };

    struct CacheLine {
        BitMap256 m_bitmap;
        CacheEntry m_entries[256]; //not init
    };

    struct CacheGroup {
        BitMap256 m_bitmap;
        CacheLine *m_lines[256]; //not init
    };

    struct CacheZone {
        BitMap256 m_bitmap;
        CacheGroup *m_groups[256]; //not init
    };

    struct Cache {
        CacheZone *m_zones[256];

        Cache() {
            memset(m_zones, 0, sizeof(m_zones));
        }
    };

    Cache m_cache;
    int m_capacity;
    int m_size;
    CacheEntry* m_entry_list_head;

    inline unsigned char IdxToZone(int key)  { return (key >> 24) & 0xff; }
    inline unsigned char IdxToGroup(int key) { return (key >> 16) & 0xff; }
    inline unsigned char IdxToLine(int key)  { return (key >>  8) & 0xff; }
    inline unsigned char IdxInLine(int key)  { return         key & 0xff; }

public:
    LRUCache(int capacity) : m_capacity(capacity), m_size(0), m_entry_list_head(NULL) {}

    int get(int key) {
        CacheZone *zone = m_cache.m_zones[IdxToZone(key)];
        if (zone == NULL) {
            return -1;
        }
        if (!zone->m_bitmap.BitExist(IdxToGroup(key))) {
            return -1;
        }

        CacheGroup *group = zone->m_groups[IdxToGroup(key)];
        if (!group->m_bitmap.BitExist(IdxToLine(key))) {
            return -1;
        }

        CacheLine *line = group->m_lines[IdxToLine(key)];
        if (!line->m_bitmap.BitExist(IdxInLine(key))) {
            return -1;
        }

        CacheEntry *entry = &line->m_entries[IdxInLine(key)];
        if (m_entry_list_head != entry) {
            entry->m_prev->m_next = entry->m_next;
            entry->m_next->m_prev = entry->m_prev;

            entry->m_next = m_entry_list_head;
            entry->m_prev = m_entry_list_head->m_prev;

            m_entry_list_head->m_prev->m_next = entry;
            m_entry_list_head->m_prev = entry;
            m_entry_list_head = entry;
        }

        return entry->m_value;
    }

    void put(int key, int value) {
        bool exist = true;
        CacheZone *zone = m_cache.m_zones[IdxToZone(key)];
        if (zone == NULL) {
            exist = false;
            zone = new CacheZone();
            m_cache.m_zones[IdxToZone(key)] = zone;
        }

        if (!zone->m_bitmap.BitExist(IdxToGroup(key))) {
            exist = false;
            zone->m_bitmap.MarkBit(IdxToGroup(key));
            zone->m_groups[IdxToGroup(key)] = new CacheGroup();
        }
        CacheGroup *group = zone->m_groups[IdxToGroup(key)];

        if (!group->m_bitmap.BitExist(IdxToLine(key))) {
            exist = false;
            group->m_bitmap.MarkBit(IdxToLine(key));
            group->m_lines[IdxToLine(key)] = new CacheLine();
        }
        CacheLine *line = group->m_lines[IdxToLine(key)];

        if (!line->m_bitmap.BitExist(IdxInLine(key))) {
            exist = false;
            line->m_bitmap.MarkBit(IdxInLine(key));
        }
        CacheEntry *entry = &line->m_entries[IdxInLine(key)];
        entry->m_key = key;
        entry->m_value = value;

        if (!exist) {
            if (m_size < m_capacity) {
                m_size++;
            }
            else {
                CacheEntry *evict_entry = m_entry_list_head->m_prev;
                if (m_size == 1) {
                    m_entry_list_head = NULL;
                }
                else {
                    evict_entry->m_prev->m_next = m_entry_list_head;
                    m_entry_list_head->m_prev = evict_entry->m_prev;
                }

                int evict_key = evict_entry->m_key;
                CacheZone *evict_zone = m_cache.m_zones[IdxToZone(evict_key)];
                CacheGroup *evict_group = evict_zone->m_groups[IdxToGroup(evict_key)];
                CacheLine *evict_line = evict_group->m_lines[IdxToLine(evict_key)];
                evict_line->m_bitmap.EraseBit(IdxInLine(evict_key));
                if (evict_line->m_bitmap.Empty()) {
                    delete evict_line;
                    //evict_group->m_lines[IdxToLine(evict_key)] = NULL;
                    evict_group->m_bitmap.EraseBit(IdxToLine(evict_key));
                    if (evict_group->m_bitmap.Empty()) {
                        delete evict_group;
                        //evict_zone->m_groups[IdxToGroup(evict_key)] = NULL;
                        evict_zone->m_bitmap.EraseBit(IdxToGroup(evict_key));
                        if (evict_zone->m_bitmap.Empty()) {
                            delete evict_zone;
                            m_cache.m_zones[IdxToZone(evict_key)] = NULL;
                        }
                    }
                }
            }
        }

        if (m_entry_list_head == NULL) {
            m_entry_list_head = entry;
            entry->m_next = entry;
            entry->m_prev = entry;
        }
        else if (m_entry_list_head != entry) {
            if (exist) {
                entry->m_prev->m_next = entry->m_next;
                entry->m_next->m_prev = entry->m_prev;
            }

            entry->m_next = m_entry_list_head;
            entry->m_prev = m_entry_list_head->m_prev;

            m_entry_list_head->m_prev->m_next = entry;
            m_entry_list_head->m_prev = entry;
            m_entry_list_head = entry;
        }
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache obj = new LRUCache(capacity);
 * int param_1 = obj.get(key);
 * obj.put(key,value);
 */

