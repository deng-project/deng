--    ____         ________     __      _        ______
--   |  _ \_      |  ______|   |  \    | |     _/  ____\
--   | | \_ \_    |  |         |   \   | |    /  _/
--   | |   \  |   |  |_____    | |\ \  | |   |  |   _____
--   | |    | |   |  ______|   | | \ \ | |   |  |  |___  |
--   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
--   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
--   |____/       |________|   |_|    \__|       \____/
--
-- __________________________________________________________
-- __________________________________________________________
--                       Project DENG
--
--
-- Copyright (C) 2020 - 2021
-- This Software is licensed under Apache License as described
-- in the LICENSE.md file, which you should have recieved with
-- this distribution.
--
-- You may reproduce and distribute copies of the
-- Work or Derivative Works thereof in any medium, with or without
-- modifications, and in Source or Object form, provided that You
-- meet the following conditions:
--
-- (a) You must give any other recipients of the Work or
--     Derivative Works a copy of this License; and
--
-- (b) You must cause any modified files to carry prominent notices
--     stating that You changed the files; and
--
-- (c) You must retain, in the Source form of any Derivative Works
--     that You distribute, all copyright, patent, trademark, and
--     attribution notices from the Source form of the Work,
--     excluding those notices that do not pertain to any part of
--     the Derivative Works; and
--
-- (d) If the Work includes a "NOTICE" text file as part of its
--     distribution, then any Derivative Works that You distribute must
--     include a readable copy of the attribution notices contained
--     within such NOTICE file, excluding those notices that do not
--     pertain to any part of the Derivative Works, in at least one
--     of the following places: within a NOTICE text file distributed
--     as part of the Derivative Works; within the Source form or
--     documentation, if provided along with the Derivative Works; or,
--     within a display generated by the Derivative Works, if and
--     wherever such third-party notices normally appear. The contents
--     of the NOTICE file are for informational purposes only and
--     do not modify the License. You may add Your own attribution
--     notices within Derivative Works that You distribute, alongside
--     or as an addendum to the NOTICE text from the Work, provided
--     that such additional attribution notices cannot be construed
--     as modifying the License.
--
-- You may add Your own copyright statement to Your modifications and
-- may provide additional or different license terms and conditions
-- for use, reproduction, or distribution of Your modifications, or
-- for any such Derivative Works as a whole, provided Your use,
-- reproduction, and distribution of the Work otherwise complies with
-- the conditions stated in this License.
--


local sandbox_data = {}


-- Copy all necessary data to build dir
function sandbox_data.datacpy()
    -- Shaders copy task
    if(package.config:sub(1,1) == '/') then
        os.execute("cp -r shaders build/")
        os.execute("cp -r textures build/")
        os.execute("cp -r assets build/")
    elseif(package.config:sub(1,1) == '\\') then
        os.execute("xcopy /s /q shaders build/shaders")
        os.execute("xcopy /s /q textures build/textures")
        os.execute("xcopy /s /q assets build/assets")
    end
end

return sandbox_data
