/*

Copyright © 2019 University of California, Berkeley

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

word_count provides lists of words and associated count

Functional methods take the head of a list as first arg.
Mutators take a reference to a list as first arg.
*/

#include "word_count.h"

/* Basic utililties */

char *new_string(char *str) {
  return strcpy((char *)malloc(strlen(str)+1), str);
}

void init_words(WordCount **wclist) {
  /* Initialize word count.  */
  *wclist = NULL;
}

size_t len_words(WordCount *wchead) {
    size_t len = 0;
    return len;
}

WordCount *find_word(WordCount *wchead, char *word) {
  /* Return count for word, if it exists */
  while (wchead != NULL) {
    if (wchead->word == word) {
      return wchead;
    } else {
      wchead = wchead->next;
    }
  }
  return NULL;
}

void add_word(WordCount **wclist, char *word) {
  /* If word is present in word_counts list, increment the count, otw insert with count 1. */
  if (find_word(wclist, word) != NULL) { //word is present in word_counts list
    WordCount *found = find_word(wclist, word);
    found->count = found->count + 1;
  } else {
    WordCount *insert;
    insert->word = word;
    insert->count = 1;
    insert->next = NULL;
    (*wclist)->next = insert;
  }
}

void fprint_words(WordCount *wchead, FILE *ofile) {
  /* print word counts to a file */
  WordCount *wc;
  printf("hi im running");
  for (wc = wchead; wc; wc = wc->next) {
    printf("runD");
    fprintf(ofile, "%i\t%s\n", wc->count, wc->word);
  }
}

void wordcount_insert_ordered(WordCount **wclist, WordCount *elem, bool less(const WordCount *, const Word Count *)) {

}

void wordcount_sort(WordCount **wclist, bool less(const WordCount *, const WordCount *)) {
  WordCount *temp;
  if (*wclist)
  if (less(*wclist,(*wclist)->next)) {
    
  } else {
    temp = (*wclist->next);
    (*wclist)->next->word = (*wclist)->word;
    (*wclist)->next->count = (*wclist)->count;
    (*wclist)->next->next = (*wclist)->next;
    
    temp->next = (*wclist);
  }
  }
}