#include "software/ai/evaluation/calc_best_shot.h"

#include <gtest/gtest.h>

#include "shared/constants.h"
#include "software/test_util/test_util.h"

TEST(CalcBestShotTest, calc_best_shot_on_enemy_goal_with_no_obstacles)
{
    World world          = ::TestUtil::createBlankTestingWorld();
    Team team            = Team(Duration::fromSeconds(1));
    Robot shooting_robot = Robot(0, Point(0, 0), Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We expect to be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(result->getPointToShootAt(),
                                               world.field().enemyGoalCenter(), 0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 12, 5);
}

TEST(CalcBestShotTest, calc_best_shot_on_friendly_goal_with_no_obstacles)
{
    World world          = ::TestUtil::createBlankTestingWorld();
    Team team            = Team(Duration::fromSeconds(1));
    Robot shooting_robot = Robot(0, Point(0, 0), Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We expect to be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(result->getPointToShootAt(),
                                               world.field().friendlyGoalCenter(), 0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 12, 5);
}

TEST(CalcBestShotTest,
     calc_best_shot_on_enemy_goal_with_obstacles_and_no_obstacles_being_ignored)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));
    Robot shooting_robot =
        Robot(0, Point(1, world.field().enemyGoalpostNeg().y()), Vector(0, 0),
              Angle::zero(), AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(
        world, {world.field().enemyGoalCenter(), Point(2.5, 0.7), Point(-1, -1)},
        Timestamp::fromSeconds(0));

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We expect to be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().enemyGoalCenter().x(), -0.3),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 6, 5);
}

TEST(CalcBestShotTest,
     calc_best_shot_on_friendly_goal_with_obstacles_and_no_obstacles_being_ignored)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));
    Robot shooting_robot =
        Robot(0, Point(-1, world.field().friendlyGoalpostNeg().y()), Vector(0, 0),
              Angle::zero(), AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateEnemyTeamState(team);

    world = ::TestUtil::setFriendlyRobotPositions(
        world, {world.field().friendlyGoalCenter(), Point(-2.5, -0.7), Point(1, 1)},
        Timestamp::fromSeconds(0));

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We expect to be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().friendlyGoalCenter().x(), -0.3),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 6, 5);
}

TEST(CalcBestShotTest,
     calc_best_shot_on_enemy_goal_with_obstacles_and_some_obstacles_being_ignored)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));
    Robot shooting_robot =
        Robot(0, Point(1, world.field().enemyGoalpostNeg().y()), Vector(0, 0),
              Angle::zero(), AngularVelocity::zero(), Timestamp::fromSeconds(0));
    Robot friendly_blocking_robot =
        Robot(1, Point(1.3, world.field().enemyGoalpostNeg().y() - 0.05), Vector(0, 0),
              Angle::zero(), AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(
        world, {world.field().enemyGoalCenter(), Point(2.5, 0.7), Point(-1, -1)},
        Timestamp::fromSeconds(0));

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We expect to be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().enemyGoalCenter().x(), -0.3),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 6, 5);
}

TEST(CalcBestShotTest,
     calc_best_shot_on_friendly_goal_with_obstacles_and_some_obstacles_being_ignored)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));
    Robot shooting_robot =
        Robot(0, Point(-1, world.field().friendlyGoalpostNeg().y()), Vector(0, 0),
              Angle::zero(), AngularVelocity::zero(), Timestamp::fromSeconds(0));
    Robot enemy_blocking_robot =
        Robot(1, Point(-1.3, world.field().enemyGoalpostNeg().y() - 0.05), Vector(0, 0),
              Angle::zero(), AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateEnemyTeamState(team);

    world = ::TestUtil::setFriendlyRobotPositions(
        world, {world.field().friendlyGoalCenter(), Point(-2.5, -0.7), Point(1, 1)},
        Timestamp::fromSeconds(0));

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We expect to be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().friendlyGoalCenter().x(), -0.3),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 6, 5);
}

TEST(CalcBestShotTest, calc_best_shot_on_enemy_goal_with_all_shots_blocked_by_obstacles)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));
    Robot shooting_robot =
        Robot(0, Point(1, world.field().enemyGoalpostNeg().y()), Vector(0, 0),
              Angle::zero(), AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(
        world, {shooting_robot.position() + Vector(ROBOT_MAX_RADIUS_METERS * 2, 0)},
        Timestamp::fromSeconds(0));

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We should not be able to find a shot
    EXPECT_FALSE(result);
}

TEST(CalcBestShotTest,
     calc_best_shot_on_friendly_goal_with_all_shots_blocked_by_obstacles)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));
    Robot shooting_robot =
        Robot(0, Point(-1, world.field().enemyGoalpostNeg().y()), Vector(0, 0),
              Angle::zero(), AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(
        world, {shooting_robot.position() - Vector(ROBOT_MAX_RADIUS_METERS * 2, 0)},
        Timestamp::fromSeconds(0));

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We should not be able to find a shot
    EXPECT_FALSE(result);
}

// https://i.imgur.com/gEJLBxx.png
TEST(CalcBestShotTest, calc_best_shot_on_friendly_goal_with_best_shot_top_segment)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(-3.641, 0.113);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateEnemyTeamState(team);

    world = ::TestUtil::setFriendlyRobotPositions(
        world, {Point(-4.50526, 0.0410256), Point(-4.49231, -0.307692)},
        Timestamp::fromSeconds(0));

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().friendlyGoalCenter().x(), 0.3),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 23, 1);
}

// https://i.imgur.com/gEJLBxx.png - reverse placements for enemy side
TEST(CalcBestShotTest, calc_best_shot_on_enemy_goal_with_best_shot_top_segment)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(3.641, 0.113);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(
        world, {Point(4.50526, 0.0410256), Point(4.49231, -0.307692)},
        Timestamp::fromSeconds(0));

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().enemyGoalCenter().x(), 0.3),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 23, 1);
}

// https://i.imgur.com/ytIABkx.png
TEST(CalcBestShotTest, calc_best_shot_on_friendly_goal_with_best_shot_middle_segment)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(-3.641, 0.113);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateEnemyTeamState(team);

    world = ::TestUtil::setFriendlyRobotPositions(
        world, {Point(-4.51282, 0.266667), Point(-4.49231, -0.307692)},
        Timestamp::fromSeconds(0));

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().friendlyGoalCenter().x(), 0),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 25, 1);
}

// https://i.imgur.com/ytIABkx.png - reverse placements for enemy side
TEST(CalcBestShotTest, calc_best_shot_on_enemy_goal_with_best_shot_middle_segment)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(3.641, 0.113);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(
        world, {Point(4.51282, 0.266667), Point(4.49231, -0.307692)},
        Timestamp::fromSeconds(0));

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().enemyGoalCenter().x(), 0),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 25, 1);
}

// https://i.imgur.com/REKh1VI.png
TEST(CalcBestShotTest, calc_best_shot_on_friendly_goal_with_best_shot_bottom_segment)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(-3.641, 0.113);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateEnemyTeamState(team);

    world = ::TestUtil::setFriendlyRobotPositions(world, {Point(-4.51282, 0.266667)},
                                                  Timestamp::fromSeconds(0));

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().friendlyGoalCenter().x(), -0.16),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 40, 1);
}

// https://i.imgur.com/REKh1VI.png - reverse placements for enemy side
TEST(CalcBestShotTest, calc_best_shot_on_enemy_goal_with_best_shot_bottom_segment)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(3.641, 0.113);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(world, {Point(4.51282, 0.266667)},
                                               Timestamp::fromSeconds(0));

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().enemyGoalCenter().x(), -0.16),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 40, 1);
}

// https://i.imgur.com/IdJqAUl.png
TEST(CalcBestShotTest, calc_best_shot_on_friendly_goal_with_overlapping_obstacles)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(-3.44615, 0.0102564);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateEnemyTeamState(team);

    world = ::TestUtil::setFriendlyRobotPositions(
        world, {Point(-4.51282, 0.266667), Point(-4.32821, 0.133333)},
        Timestamp::fromSeconds(0));

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().friendlyGoalCenter().x(), -0.21),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 28, 1);
}

// https://i.imgur.com/IdJqAUl.png - reverse placements for enemy side
TEST(CalcBestShotTest, calc_best_shot_on_enemy_goal_with_overlapping_obstacles)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(3.44615, 0.0102564);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(
        world, {Point(4.51282, 0.266667), Point(4.32821, 0.133333)},
        Timestamp::fromSeconds(0));

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().enemyGoalCenter().x(), -0.21),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 28, 1);
}

// https://i.imgur.com/EVbjBlb.png
TEST(CalcBestShotTest, calc_best_shot_on_friendly_goal_with_negative_y_ball_placement)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(-3.50769, -1.00513);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateEnemyTeamState(team);

    world = ::TestUtil::setFriendlyRobotPositions(world, {Point(-4.51282, 0.266667)},
                                                  Timestamp::fromSeconds(0));

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().friendlyGoalCenter().x(), -0.23),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 21.5, 1);
}

// https://i.imgur.com/EVbjBlb.png - reverse placements for enemy side
TEST(CalcBestShotTest, calc_best_shot_on_enemy_goal_with_negative_y_ball_placement)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(3.50769, -1.00513);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(world, {Point(4.51282, 0.266667)},
                                               Timestamp::fromSeconds(0));

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().enemyGoalCenter().x(), -0.23),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 21.5, 1);
}

// https://i.imgur.com/KtrR6iP.png
TEST(CalcBestShotTest, calc_best_shot_on_friendly_goal_with_positive_y_ball_placement)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(-3.49744, 0.984615);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateEnemyTeamState(team);

    world = ::TestUtil::setFriendlyRobotPositions(world, {Point(-4.50256, -0.225641)},
                                                  Timestamp::fromSeconds(0));

    auto result = calcBestShotOnGoal(world.field(), world.friendlyTeam(),
                                     world.enemyTeam(), shooting_robot.position(),
                                     TeamType::FRIENDLY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().friendlyGoalCenter().x(), 0.24),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 21, 1);
}

// https://i.imgur.com/KtrR6iP.png - reverse placements for enemy side
TEST(CalcBestShotTest, calc_best_shot_on_enemy_goal_with_positive_y_ball_placement)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(3.49744, 0.984615);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                 AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(world, {Point(4.50256, -0.225641)},
                                               Timestamp::fromSeconds(0));

    auto result =
        calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                           shooting_robot.position(), TeamType::ENEMY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_TRUE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        result->getPointToShootAt(), Point(world.field().enemyGoalCenter().x(), 0.24),
        0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 21, 1);
}

// https://i.imgur.com/KtrR6iP.png - reverse placements for enemy side
TEST(CalcBestShotTest, calc_best_shot_on_friendly_goal_outlier)
{
    World world = ::TestUtil::createBlankTestingWorld();
    Team team   = Team(Duration::fromSeconds(1));

    Point shooting_robot_pos = Point(0.348718, 2.69744);
    Robot shooting_robot     = Robot(0, shooting_robot_pos, Vector(0, 0), Angle::zero(),
                                     AngularVelocity::zero(), Timestamp::fromSeconds(0));
    team.updateRobots({shooting_robot});
    world.updateFriendlyTeamState(team);

    world = ::TestUtil::setEnemyRobotPositions(world, {Point(-3.29231, -0.215385), Point(-3.48021, 0.407988), Point(-3.31282, 0.882051), Point(-1.2, 1.86667), Point(-3.45938, 0.231164)},
                                               Timestamp::fromSeconds(0));

    auto result =
            calcBestShotOnGoal(world.field(), world.friendlyTeam(), world.enemyTeam(),
                               shooting_robot.position(), TeamType::FRIENDLY, {shooting_robot});

    // We should not be able to find a shot
    ASSERT_FALSE(result);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
            result->getPointToShootAt(), Point(world.field().enemyGoalCenter().x(), 0.24),
            0.05));
    EXPECT_NEAR(result->getOpenAngle().toDegrees(), 21, 1);
}

//TEST(AngleMapTest, add_obstacle_angle_segment_takes_entire_angle_map)
//{
//    Angle top_angle    = Angle::half();
//    Angle bottom_angle = Angle::zero();
//    AngleMap angle_map = AngleMap(top_angle, bottom_angle, 1);
//
//    ObstacleAngleSegment obstacle_angle_seg =
//        ObstacleAngleSegment(top_angle, bottom_angle);
//    angle_map.addObstacleAngleSegment(obstacle_angle_seg);
//
//    EXPECT_EQ(0, angle_map.getBiggestViableAngleSegment().getDelta());
//}
//
//TEST(AngleMapTest, add_obstacle_angle_segment_takes_lower_half_angle_map)
//{
//    Angle top_angle    = Angle::half();
//    Angle bottom_angle = Angle::zero();
//    AngleMap angle_map = AngleMap(top_angle, bottom_angle, 1);
//
//    ObstacleAngleSegment obstacle_angle_seg =
//        ObstacleAngleSegment(Angle::quarter(), Angle::zero());
//    angle_map.addObstacleAngleSegment(obstacle_angle_seg);
//
//    EXPECT_EQ(90, angle_map.getBiggestViableAngleSegment().getDelta());
//}
//
//TEST(AngleMapTest, add_obstacle_angle_segment_takes_upper_half_angle_map)
//{
//    Angle top_angle    = Angle::half();
//    Angle bottom_angle = Angle::zero();
//    AngleMap angle_map = AngleMap(top_angle, bottom_angle, 1);
//
//    ObstacleAngleSegment obstacle_angle_seg =
//        ObstacleAngleSegment(Angle::half(), Angle::quarter());
//    angle_map.addObstacleAngleSegment(obstacle_angle_seg);
//
//    EXPECT_EQ(90, angle_map.getBiggestViableAngleSegment().getDelta());
//}
//
//TEST(AngleMapTest, add_obstacle_angle_segment_contained_within_another)
//{
//    Angle top_angle    = Angle::half();
//    Angle bottom_angle = Angle::zero();
//    AngleMap angle_map = AngleMap(top_angle, bottom_angle, 1);
//
//    ObstacleAngleSegment obstacle_angle_seg =
//        ObstacleAngleSegment(Angle::quarter(), Angle::zero());
//    angle_map.addObstacleAngleSegment(obstacle_angle_seg);
//
//    ObstacleAngleSegment overlapping_angle_seg =
//        ObstacleAngleSegment(Angle::fromDegrees(45), Angle::zero());
//    angle_map.addObstacleAngleSegment(overlapping_angle_seg);
//
//    EXPECT_EQ(90, angle_map.getBiggestViableAngleSegment().getDelta());
//}
//
//TEST(AngleMapTest, add_obstacle_angle_segment_overlaps_with_another)
//{
//    Angle top_angle    = Angle::half();
//    Angle bottom_angle = Angle::zero();
//    AngleMap angle_map = AngleMap(top_angle, bottom_angle, 1);
//
//    ObstacleAngleSegment obstacle_angle_seg =
//        ObstacleAngleSegment(Angle::quarter(), Angle::zero());
//    angle_map.addObstacleAngleSegment(obstacle_angle_seg);
//
//    ObstacleAngleSegment overlapping_angle_seg =
//        ObstacleAngleSegment(Angle::fromDegrees(135), Angle::fromDegrees(45));
//    angle_map.addObstacleAngleSegment(overlapping_angle_seg);
//
//    EXPECT_EQ(45, angle_map.getBiggestViableAngleSegment().getDelta());
//}
