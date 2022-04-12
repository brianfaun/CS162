/*
 * Implementation of the word_count interface using Pintos lists and pthreads.
 *
 * You may modify this file, and are expected to modify it.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_lp.c"
#endif

#ifndef PTHREADS
#error "PTHREADS must be #define'd when compiling word_count_lp.c"
#endif

#include "word_count.h"

void init_words(word_count_list_t* wclist) { 
  list_init(&(wclist->lst));
  pthread_mutex_init(&wclist->lock, NULL);
}

size_t len_words(word_count_list_t* wclist) {
  pthread_mutex_lock(&wclist->lock);
  size_t length = 0;
  struct list_elem *i;
  for (i = list_begin(&(wclist->lst)); i != list_end(&(wclist->lst)); i = list_next (i)) {
    length += 1u;
  }
  pthread_mutex_unlock(&wclist->lock);
  return length;
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  struct list_elem *i;
  for (i = list_begin(&(wclist->lst)); i != list_end(&(wclist->lst)); i = list_next(i)) {
    word_count_t *findWord = list_entry(i, word_count_t, elem);
    if (strcmp(findWord->word, word) == 0) {
      return findWord;
    }
  }
  return NULL;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
  pthread_mutex_lock(&wclist->lock);
  word_count_t *addWord = find_word(&(wclist->lst), word);
  if (addWord != NULL) {
    addWord->count += 1;
    pthread_mutex_unlock(&wclist->lock);
    return addWord;
  } else {  //addWord is NULL
    addWord = malloc(sizeof(word_count_t));
    addWord->word = word;
    addWord->count = 1;
    list_push_back(&(wclist->lst), &(addWord->elem));
    pthread_mutex_unlock(&wclist->lock);
    return addWord;
  }
  pthread_mutex_unlock(&wclist->lock);
  return NULL;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) {
  pthread_mutex_lock(&wclist->lock);
  struct list_elem *i;
  word_count_t *printWord;
  for (i = list_begin(&(wclist->lst)); i != list_end(&(wclist->lst)); i = list_next(i)) {
    printWord = list_entry(i, word_count_t, elem);
    fprintf(outfile, "%8d\t%s\n", printWord->count, printWord->word);
  }
  pthread_mutex_unlock(&wclist->lock);
}

static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) {
  bool (*compare) (const word_count_t *, const word_count_t *) = aux;
  word_count_t *firstWord = list_entry(ewc1, word_count_t, elem);
  word_count_t *secondWord = list_entry(ewc2, word_count_t, elem);
  return compare(firstWord, secondWord);
}

void wordcount_sort(word_count_list_t* wclist, bool less(const word_count_t*, const word_count_t*)) {
  pthread_mutex_lock(&wclist->lock);
  list_sort(&(wclist->lst), less_list, less);
  pthread_mutex_unlock(&wclist->lock);
}