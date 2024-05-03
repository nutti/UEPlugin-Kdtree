/*!
 * FunctionalTest
 *
 * Copyright (c) 2019-2023 nutti
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

using UnrealBuildTool;
using System.Collections.Generic;

public class FunctionalTestEditorTarget : TargetRules
{
	public FunctionalTestEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.Add("FunctionalTest");
	}
}
