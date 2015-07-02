
#include "rng.h"
#include "bittable.h"

  void
init2_URandom (URandom* urandom, uint pcidx, uint npcs)
{
  (void) npcs;
  {uint i = 0;for (; i < ArraySz(urandom->state); ++i) {
    uint32 x = i + ArraySz(urandom->state) * pcidx;
    urandom->state[i] = uint32_hash_ThomasWang (x);
  }}

  urandom->state[0] = pcidx; 
  Randomize( urandom->state );
  init_WELL512 (urandom);
  urandom->salt = uint32_hash_ThomasWang (pcidx);
}

  uint32
uint32_URandom (URandom* urandom)
{
  uint32 x = uint32_WELL512 (urandom);
  return (x ^ urandom->salt);
}

/** Generate a uint in {0,...,n-1}.**/
  uint
uint_URandom (URandom* urandom, uint n)
{
#if 1
  uint x = uint32_URandom (urandom);
  return (uint) (n * (x * 2.328306e-10));
#else
  /* Probably screws with the randomness.*/
  const uint32 q = (Max_uint32 / n);
  const uint32 m = Max_uint32 - (Max_uint32 % n);
  uint32 x;
  do {
    x = uint32_URandom (a);
  } while (x >= m);
  return x / q;
#endif
}


/** Shuffle integers in an array.**/
  void
shuffle_uints_URandom (URandom* urandom, uint* a, uint n)
{
  for (; n > 1; --n)
  {
    uint i = n-1;
    uint j = uint_URandom (urandom, n);
    SwapT( uint, a[i], a[j] );
  }
}

  uint
randommod_sysCx(uint n)
{
  const uint max = n-1;
  FixDeclBitTable( bt, NBits_uint, 0 );
  const uint nbits = lg_ujint (max) + 1;
  const uint nbytes = ceil_quot(nbits, NBits_byte);
  uint x;

  do {
    randomize_sysCx (bt.s, nbytes);

    /* We can assume each bit is uniformly random,
     * so truncate as much as possible without excluding {max}.
     */
    x = get_uint_BitTable (bt, 0, nbits);

    /* If {x} is outside of our range, then try again.
     * This has less than a 50% chance of happening, so this loop will terminate eventually.
     */
  } while (x > max);

  return x;
}

