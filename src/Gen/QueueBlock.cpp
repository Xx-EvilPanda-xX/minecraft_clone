#include "QueueBlock.h"

bool operator== (const QueueBlock& qb1, const QueueBlock& qb2)
{
	return (qb1.loc.containingChunk == qb2.loc.containingChunk) &&
		(qb1.loc.sectionIndex == qb2.loc.sectionIndex) &&
		(qb1.loc.worldPos == qb2.loc.worldPos) &&
		(qb1.sectionRelativePos == qb2.sectionRelativePos);
}