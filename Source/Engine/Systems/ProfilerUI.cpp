#include "Precomp.h"
#include "ProfilerUI.h"

#include "../Input.h"
#include <Core/Utils.h>

#include <sstream>

void ProfilerUI::Draw()
{
	if (Input::GetKeyPressed(Input::Keys::F2))
	{
		myShouldDraw = !myShouldDraw;
	}

	if (!myShouldDraw)
	{
		return;
	}

	ImGui::Begin("Profiler", &myShouldDraw);

	if (ImGui::Button("Buffer captures"))
	{
		const auto& frameData = Profiler::GetInstance().GetBufferedFrameData();
		for (const Profiler::FrameProfile& frameProfile : frameData)
		{
			myFramesToRender.push_back(std::move(ProcessFrameProfile(frameProfile)));
		}
	}
	if (ImGui::Button("Clear captures"))
	{
		myFramesToRender.clear();
	}

	bool plotWindowHovered = false;
	char nodeName[64];
	for (const FrameData& frameData : myFramesToRender)
	{
		sprintf(nodeName, "Frame %llu", frameData.myFrameProfile.myFrameNum);
		if (ImGui::TreeNode(nodeName))
		{
			// Precalculate the whole height
			float totalHeight = kMarkHeight;
			for (const auto& threadMaxLevelPair : frameData.myMaxLevels)
			{
				totalHeight += (threadMaxLevelPair.second + 1) * kMarkHeight;
			}
			totalHeight += ImGui::GetStyle().ScrollbarSize;

			ImGui::BeginChild(nodeName, { 0,totalHeight });
			plotWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

			DrawThreadColumn(frameData, totalHeight);
			ImGui::SameLine();
			DrawMarksColumn(frameData, totalHeight);

			ImGui::EndChild();
			ImGui::TreePop();
		}
	}
	if (plotWindowHovered)
	{
		myWidthScale += Input::GetMouseWheelDelta() * 0.1f;
		myWidthScale = std::max(myWidthScale, 1.0f);
	}

	ImGui::End();
}

void ProfilerUI::DrawThreadColumn(const FrameData& aFrameData, float aTotalHeight) const
{
	char name[64];
	std::sprintf(name, "%llu##ThreadColumn", aFrameData.myFrameProfile.myFrameNum);
	ImGui::BeginChild(name, { kThreadColumnWidth, aTotalHeight });
	ImGui::SetCursorPosX(0);
	ImGui::SetNextItemWidth(kThreadColumnWidth);
	ImGui::Text("Thread Name");
	float yOffset = kMarkHeight;
	for (const auto& threadIdLevelPair : aFrameData.myMaxLevels)
	{
		const std::thread::id threadId = threadIdLevelPair.first;
		const uint32_t maxLevel = threadIdLevelPair.second;
		const float height = (maxLevel + 1) * kMarkHeight;

		ImGui::SetCursorPos({ 0, yOffset });
		std::ostringstream stringStream;
		stringStream << "Thread " << threadId;
		std::string threadString = stringStream.str();
		ImGui::Button(threadString.c_str(), { kThreadColumnWidth, height });
		yOffset += height;
	}
	ImGui::EndChild();
}

void ProfilerUI::DrawMarksColumn(const FrameData& aFrameData, float aTotalHeight) const
{
	// Unresizing scrollable parent window
	char name[64];
	std::sprintf(name, "%llu##MarksColumn", aFrameData.myFrameProfile.myFrameNum);
	ImGui::BeginChild(name, { 0, aTotalHeight }, false, ImGuiWindowFlags_HorizontalScrollbar);

	// Zoomable child window
	const float fixedWindowWidth = ImGui::GetWindowWidth();
	float plotWidth = fixedWindowWidth * myWidthScale;
	plotWidth = std::max(plotWidth, fixedWindowWidth);
	std::sprintf(name, "%llu##ZoomWindow", aFrameData.myFrameProfile.myFrameNum);
	ImGui::BeginChild(name, { plotWidth, aTotalHeight - ImGui::GetStyle().ScrollbarSize });

	// top bar for frame
	const long long frameDuration = (aFrameData.myFrameProfile.myEndStamp - aFrameData.myFrameProfile.myBeginStamp).count();
	std::sprintf(name, "Duration: %lld", frameDuration);
	ImGui::SetCursorPosX(0);
	ImGui::SetNextItemWidth(kThreadColumnWidth);
	ImGui::Button(name, { plotWidth, kMarkHeight });

	auto InverseLerpProfile = [
		min = aFrameData.myFrameProfile.myBeginStamp.time_since_epoch().count(),
		max = aFrameData.myFrameProfile.myEndStamp.time_since_epoch().count()
	](auto val) { return (val - min) / static_cast<float>(max - min); };

	float yOffset = kMarkHeight;
	for (const auto& threadIdLevelPair : aFrameData.myMaxLevels)
	{
		const std::thread::id threadId = threadIdLevelPair.first;
		const uint32_t maxLevel = threadIdLevelPair.second;
		const float widthPerDurationRatio = plotWidth / frameDuration;
		
		const MarksVec& threadMarks = aFrameData.myThreadMarkMap.at(threadId);
		const HierarchyMap& hierarchy = aFrameData.myThreadHierarchyMap.at(threadId);
		for (const Profiler::Mark& mark : threadMarks)
		{
			const long long markDuration = (mark.myEndStamp - mark.myBeginStamp).count();
			const long long startTimeOffset = (mark.myBeginStamp - aFrameData.myFrameProfile.myBeginStamp).count();

			const float x = plotWidth * InverseLerpProfile(mark.myBeginStamp.time_since_epoch().count());
			const float y = yOffset + hierarchy.at(mark.myId) * kMarkHeight;
			const float width = widthPerDurationRatio * markDuration;
			ImGui::SetCursorPos({ x, y });

			std::sprintf(name, "%s - %lldns", mark.myName, markDuration);
			ImGui::Button(name, { width, kMarkHeight });
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Name: %s\nDuration: %lldns\nId: %d\nParent Id: %d",
					mark.myName, markDuration, mark.myId, mark.myParentId);
			}
		}
		yOffset += (maxLevel + 1) * kMarkHeight;
	}
	ImGui::EndChild();
	ImGui::EndChild();
}

ProfilerUI::HierarchyMap ProfilerUI::CalculateHierarchy(const MarksVec& aMarks, uint32_t& aMaxLevel)
{
	// we need to figure out how to display the stacked bars representing
	// marks, so for that we will calculate the nested-level of every Mark
	HierarchyMap hierarchyMap;
	hierarchyMap.reserve(aMarks.size());
	// While we're at it, calculate the max nested level, so that we can determine
	// the height of hierarchy
	aMaxLevel = 0;
	for (const Profiler::Mark& mark : aMarks)
	{
		if (mark.myParentId == -1)
		{
			// no parent -> initial level
			hierarchyMap[mark.myId] = 0;
		}
		else
		{
			// parent exists -> parents level + 1
			uint32_t newLevel = hierarchyMap[mark.myParentId] + 1;
			hierarchyMap[mark.myId] = newLevel;
			// update the max level
			aMaxLevel = std::max(aMaxLevel, newLevel);
		}
	}
	return hierarchyMap;
}

ProfilerUI::FrameData ProfilerUI::ProcessFrameProfile(const Profiler::FrameProfile& aProfile)
{
	FrameData data;
	data.myThreadMarkMap = Utils::GroupBy<ThreadMarkMap>(aProfile.myFrameMarks,
		[](const Profiler::Mark& aMark) { return aMark.myThreadId; }
	);
	for (const auto& threadMarksPair : data.myThreadMarkMap)
	{
		uint32_t maxLevel = 0;
		data.myThreadHierarchyMap[threadMarksPair.first] = std::move(CalculateHierarchy(threadMarksPair.second, maxLevel));
		data.myMaxLevels[threadMarksPair.first] = maxLevel;
	}
	data.myFrameProfile = std::move(aProfile);
	return data;
}