// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: TreeItemWrappers.h - wrappers model loader classes in wxTreeItemData hierarchy
// author: Karl-Mihkel Ott

#ifndef TREE_ITEM_NODE_H
#define TREE_ITEM_NODE_H

namespace DENG {
	namespace Editor {

		enum TreeItemType {
			TREE_ITEM_TYPE_MODEL,
			TREE_ITEM_TYPE_SCENE,
			TREE_ITEM_TYPE_NODE,
			TREE_ITEM_TYPE_SKELETON,
			TREE_ITEM_TYPE_MESH,
			TREE_ITEM_TYPE_ANIMATION,
		};


		class TreeItem : public wxTreeItemData {
			private:
				TreeItemType m_type;
				
			public:
				TreeItem(TreeItemType _type) :
					m_type(_type) {}

				inline TreeItemType GetType() {
					return m_type;
				}
		};

		
		class TreeItemScene : public TreeItem {
			private:
				SceneLoader* m_loader = nullptr;

			public:
				TreeItemScene(SceneLoader* _loader) :
					TreeItem(TREE_ITEM_TYPE_SCENE),
					m_loader(_loader) {}

				inline SceneLoader* GetSceneLoader() {
					return m_loader;
				}
		};

		
		class TreeItemNode : public TreeItem {
			private:
				NodeLoader* m_loader = nullptr;

			public:
				TreeItemNode(NodeLoader* _loader) :
					TreeItem(TREE_ITEM_TYPE_NODE),
					m_loader(_loader) {}

				inline NodeLoader* GetNodeLoader() {
					return m_loader;
				}
		};


		class TreeItemSkeleton : public TreeItem {
			private:
				SkeletonLoader* m_loader = nullptr;

			public:
				TreeItemSkeleton(SkeletonLoader *_loader) :
					TreeItem(TREE_ITEM_TYPE_SKELETON),
					m_loader(_loader) {}

				inline SkeletonLoader* GetLoader() {
					return m_loader;
				}
		};


		class TreeItemMesh : public TreeItem {
			private:
				MeshLoader* m_loader = nullptr;

			public:
				TreeItemMesh(MeshLoader* _loader) :
					TreeItem(TREE_ITEM_TYPE_MESH),
					m_loader(_loader) {}

				inline MeshLoader* GetMeshLoader() {
					return m_loader;
				}
		};


		class TreeItemAnimation : public TreeItem {
			private:
				Animation* m_animation = nullptr;

			public:
				TreeItemAnimation(Animation *_animation) :
					TreeItem(TREE_ITEM_TYPE_ANIMATION),
					m_animation(_animation) {}

				inline Animation* GetAnimation() {
					return m_animation;
				}
		};
	}
}

#endif