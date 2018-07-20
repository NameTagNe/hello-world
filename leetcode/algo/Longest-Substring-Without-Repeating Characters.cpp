// https://leetcode.com/problems/longest-substring-without-repeating-characters/description/
// 12ms



class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int max_len = 0;
        int char_idx[256];
        memset(char_idx, 0xff, sizeof(char_idx));
        int substr_head_idx = 0;
        for (unsigned idx = 0; idx < s.length(); ++idx) {
            int collision_idx = char_idx[s[idx]];
            if (collision_idx >= 0) {
                int len = idx - substr_head_idx;
                if (len > max_len) {
                    max_len = len;
                }
                while (substr_head_idx <= collision_idx) {
                    char_idx[s[substr_head_idx++]] = -1;
                }
            }
            char_idx[s[idx]] = idx;
        }
        int len = s.length() - substr_head_idx;
        return max_len > len ? max_len : len;
    }
};
