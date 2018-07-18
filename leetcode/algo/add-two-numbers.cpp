// https://leetcode.com/problems/add-two-numbers/description/
// 28ms

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode *l1head = l1;
        ListNode *l2head = l2;
        int carry = 0;
        while (true) {
            l1->val += l2->val + carry;
            carry = 0;
            if (l1->val >= 10) {
                l1->val -= 10;
                carry = 1;
            }
            if (l1->next == NULL || l2->next == NULL) {
                break;
            }
            l1 = l1->next;
            l2 = l2->next;
        }
        
        if (carry == 0) {
            if (l1->next == NULL) {
                l1->next = l2->next;
            }
        }
        else {
            if (l1->next == NULL && l2->next == NULL) {
                l1->next = l2head;
                l2head->val = 1;
                l2head->next = NULL;
                return l1head;
            }
            
            ListNode *l = NULL;
            if (l1->next != NULL) {
                l = l1->next;
            }
            else {
                l1->next = l2->next;
                l = l2->next;
            }

            while (true) {
                l->val += 1;
                if (l->val >= 10) {
                    l->val -= 10;
                }
                else {
                    break;
                }
                if (l->next == NULL) {
                    l->next = l2head;
                    l2head->val = 1;
                    l2head->next = NULL;
                    break;
                }
                l = l->next;
            }
        }
        
        return l1head;
    }
};
