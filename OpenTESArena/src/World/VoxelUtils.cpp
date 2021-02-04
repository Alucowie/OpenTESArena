#include "ChunkUtils.h"
#include "VoxelFacing2D.h"
#include "VoxelFacing3D.h"
#include "VoxelUtils.h"

#include "components/debug/Debug.h"

NewInt2 VoxelUtils::originalVoxelToNewVoxel(const OriginalInt2 &voxel)
{
	return NewInt2(voxel.y, voxel.x);
}

OriginalInt2 VoxelUtils::newVoxelToOriginalVoxel(const NewInt2 &voxel)
{
	return VoxelUtils::originalVoxelToNewVoxel(voxel);
}

Double2 VoxelUtils::getTransformedVoxel(const Double2 &voxel)
{
	return Double2(voxel.y, voxel.x);
}

NewInt3 VoxelUtils::chunkVoxelToNewVoxel(const ChunkInt2 &chunk, const VoxelInt3 &voxel)
{
	const NewInt3 baseVoxel(chunk.x * ChunkUtils::CHUNK_DIM, 0, chunk.y * ChunkUtils::CHUNK_DIM);
	return baseVoxel + voxel;
}

NewInt3 VoxelUtils::chunkCoordToNewVoxel(const ChunkCoord3D &coord)
{
	return VoxelUtils::chunkVoxelToNewVoxel(coord.chunk, coord.voxel);
}

NewInt2 VoxelUtils::chunkVoxelToNewVoxel(const ChunkInt2 &chunk, const VoxelInt2 &voxel)
{
	return (chunk * ChunkUtils::CHUNK_DIM) + voxel;
}

ChunkCoord3D VoxelUtils::newVoxelToChunkCoord(const NewInt3 &voxel)
{
	ChunkCoord3D coord(
		ChunkInt2(voxel.x / ChunkUtils::CHUNK_DIM, voxel.z / ChunkUtils::CHUNK_DIM),
		VoxelInt3(voxel.x % ChunkUtils::CHUNK_DIM, voxel.y, voxel.z % ChunkUtils::CHUNK_DIM));
	return coord;
}

ChunkCoord2D VoxelUtils::newVoxelToChunkCoord(const NewInt2 &voxel)
{
	// @todo: need to handle voxel outside grid.
	// @todo: probably want (int)Floor() instead of modulo.

	ChunkCoord2D coord(
		ChunkInt2(voxel.x / ChunkUtils::CHUNK_DIM, voxel.y / ChunkUtils::CHUNK_DIM),
		VoxelInt2(voxel.x % ChunkUtils::CHUNK_DIM, voxel.y % ChunkUtils::CHUNK_DIM));
	return coord;
}

ChunkCoord2D VoxelUtils::levelVoxelToChunkCoord(const LevelInt2 &voxel)
{
	// @todo: make sure it handles negative coordinates.
	return VoxelUtils::newVoxelToChunkCoord(voxel);
}

ChunkInt2 VoxelUtils::newVoxelToChunk(const NewInt2 &voxel)
{
	const ChunkCoord2D chunkCoord = VoxelUtils::newVoxelToChunkCoord(voxel);
	return chunkCoord.chunk;
}

VoxelInt2 VoxelUtils::wrapVoxelCoord(const VoxelInt2 &voxel)
{
	// @todo: handle negative numbers
	return VoxelInt2(voxel.x % ChunkUtils::CHUNK_DIM, voxel.y % ChunkUtils::CHUNK_DIM);
}

Double2 VoxelUtils::getVoxelCenter(const Int2 &voxel)
{
	return Double2(
		static_cast<double>(voxel.x) + 0.50,
		static_cast<double>(voxel.y) + 0.50);
}

Double3 VoxelUtils::getNormal(VoxelFacing2D facing)
{
	if (facing == VoxelFacing2D::PositiveX)
	{
		return Double3::UnitX;
	}
	else if (facing == VoxelFacing2D::NegativeX)
	{
		return -Double3::UnitX;
	}
	else if (facing == VoxelFacing2D::PositiveZ)
	{
		return Double3::UnitZ;
	}
	else if (facing == VoxelFacing2D::NegativeZ)
	{
		return -Double3::UnitZ;
	}
	else
	{
		DebugUnhandledReturnMsg(Double3, std::to_string(static_cast<int>(facing)));
	}
}

VoxelFacing3D VoxelUtils::convertFaceTo3D(VoxelFacing2D facing)
{
	if (facing == VoxelFacing2D::PositiveX)
	{
		return VoxelFacing3D::PositiveX;
	}
	else if (facing == VoxelFacing2D::NegativeX)
	{
		return VoxelFacing3D::NegativeX;
	}
	else if (facing == VoxelFacing2D::PositiveZ)
	{
		return VoxelFacing3D::PositiveZ;
	}
	else if (facing == VoxelFacing2D::NegativeZ)
	{
		return VoxelFacing3D::NegativeZ;
	}
	else
	{
		DebugUnhandledReturnMsg(VoxelFacing3D, std::to_string(static_cast<int>(facing)));
	}
}

std::optional<VoxelFacing2D> VoxelUtils::tryConvertFaceTo2D(VoxelFacing3D facing)
{
	if (facing == VoxelFacing3D::PositiveX)
	{
		return VoxelFacing2D::PositiveX;
	}
	else if (facing == VoxelFacing3D::NegativeX)
	{
		return VoxelFacing2D::NegativeX;
	}
	else if (facing == VoxelFacing3D::PositiveZ)
	{
		return VoxelFacing2D::PositiveZ;
	}
	else if (facing == VoxelFacing3D::NegativeZ)
	{
		return VoxelFacing2D::NegativeZ;
	}
	else if ((facing == VoxelFacing3D::PositiveY) || (facing == VoxelFacing3D::NegativeY))
	{
		return std::nullopt;
	}
	else
	{
		DebugUnhandledReturnMsg(VoxelFacing2D, std::to_string(static_cast<int>(facing)));
	}
}

void VoxelUtils::getSurroundingVoxels(const VoxelInt3 &voxel, int distance, VoxelInt3 *outMinVoxel, VoxelInt3 *outMaxVoxel)
{
	DebugAssert(distance >= 0);
	*outMinVoxel = VoxelInt3(voxel.x - distance, voxel.y - distance, voxel.z - distance);
	*outMaxVoxel = VoxelInt3(voxel.x + distance, voxel.y + distance, voxel.z + distance);
}

void VoxelUtils::getSurroundingVoxels(const VoxelInt2 &voxel, int distance, VoxelInt2 *outMinVoxel, VoxelInt2 *outMaxVoxel)
{
	DebugAssert(distance >= 0);
	*outMinVoxel = VoxelInt2(voxel.x - distance, voxel.y - distance);
	*outMaxVoxel = VoxelInt2(voxel.x + distance, voxel.y + distance);
}
