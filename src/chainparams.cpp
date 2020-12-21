// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2018-2019 The KFX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "libzerocoin/Params.h"
#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
    (0, uint256("00000c7631ac91c23591067aaa6d4769f354b34ea74dd91e54754ba147afe1c8"))
	(200, uint256("0000000b39a8f13e2b592ed096f416fff07cd05b1df54dbb42bd30057e2ea6fa"))
	(500, uint256("418c6eebaef48a5291d3992ec73c06d16fafa61a308cb0c1d0303722589cddc9"))
	(1000, uint256("df0d3f9a18c911727aa3d7e1d8cf39827ed40a90ac33c4180cfd4651cd2e5eec"))
	(4000, uint256("699bbeb5346c74b4a6c75da835b5f39645e38b1be984d9c7e6b7ac61047e8f20"))
    ;
static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1608571371, // * UNIX timestamp of last checkpoint block
    7822,    	// * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    3000        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of(0, uint256("00000c7631ac91c23591067aaa6d4769f354b34ea74dd91e54754ba147afe1c8"));
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1607871600,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("00000c7631ac91c23591067aaa6d4769f354b34ea74dd91e54754ba147afe1c8"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1607871600,
    0,
    100};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params() const
{
    assert(this);
    static CBigNum bnTrustedModulus(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParams = libzerocoin::ZerocoinParams(bnTrustedModulus);

    return &ZCParams;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x01;
        pchMessageStart[1] = 0x1c;
        pchMessageStart[2] = 0x52;
        pchMessageStart[3] = 0xbc;
        vAlertPubKey = ParseHex("046e1de0e6a1117f16485c59a34e74e29fa40ca8ac9fdb8c1dc9e440f8bec278c0552fd8123c8494c0e705108281382da54e5c00a911f65b9989576d58a67ed1f6");
        nDefaultPort = 29929;
        bnProofOfWorkLimit = ~uint256(0) >> 20; // KFX starting difficulty is 1 / 2^12
        nSubsidyHalvingInterval = 4000000;
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 2 * 60;
        nTargetSpacing = 2 * 60;
        nMaturity = 60;
        nMasternodeCountDrift = 20;
        nMaxMoneyOut = 5000000 * COIN;

        /** Height or Time Based Activations **/
        nLastPOWBlock = 200;
        nModifierUpdateBlock = 1;
        nZerocoinStartHeight = 101;
        nAccumulatorStartHeight = 50;
        nZerocoinStartTime = 1529726034;
        nBlockEnforceSerialRange = 1;
        nBlockRecalculateAccumulators = ~1;
        nBlockFirstFraudulent = ~1;
        nBlockLastGoodCheckpoint = ~1;

        const char* pszTimestamp = "KFX chain swap on VAULT Investments - Dec 2020";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 0 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1607871600; // UTC May 1 2020 00:00:00
        genesis.nBits = 0x1E0FFFF0;
        genesis.nNonce = 1450468;

		    hashGenesisBlock = genesis.GetHash();

        if(genesis.GetHash() != uint256("00000c7631ac91c23591067aaa6d4769f354b34ea74dd91e54754ba147afe1c8"))
        assert(hashGenesisBlock == uint256("00000c7631ac91c23591067aaa6d4769f354b34ea74dd91e54754ba147afe1c8"));
        assert(genesis.hashMerkleRoot == uint256("7fcc865db63a63a0a216c742ec4cc5e60aef213572d7758674c48f8b6d0f6144"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 45);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 8);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 215);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0x0D)(0x2A)(0xA3).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0xD1)(0x33)(0x01).convert_to_container<std::vector<unsigned char> >();
        // BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x02)(0x8A).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        strSporkKey = "046e1de0e6a1117f16485c59a34e74e29fa40ca8ac9fdb8c1dc9e440f8bec278c0552fd8123c8494c0e705108281382da54e5c00a911f65b9989576d58a67ed1f6";
        strObfuscationPoolDummyAddress = "KJK3gzNgm9RLsTVshiTdqqywNqo27tPrsr";
        nStartMasternodePayments = 1516371317;

        /** Zerocoin */
        zerocoinModulus = "09967519546633664161786592116176188244477851735247464471262506936419821425521535537971729591952844"
            "7993474316710383495651554441492284699784318317920768642771782479156335915769589065125381511877736478656163146112"
            "0274778749282514833787626065783543239214817635079109932062526589069357037529741126387254431354640890113819012884"
            "5984727198875027923684216536572545251927687617204175968296261497488347174940599536643486971848573156659145626196"
            "7823585489859811013117353687337255659905995864128249363763371577762942618252152391331762957988146692582388256324"
            "83429163614150572996214543792012965484479369325464648385288082629456952";
        nMaxZerocoinSpendsPerTransaction = 7;
        nMinZerocoinMintFee = 1 * ZCENT;
        nMintRequiredConfirmations = 20;
        nRequiredAccumulation = 1;
        nDefaultSecurityLevel = 100;
        nZerocoinHeaderVersion = 4;
        nBudget_Fee_Confirmations = 6;

        nStakeMinConfirmations = 720;
        nStakeMinAmount = 50 * COIN;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xfb;
        pchMessageStart[2] = 0xfc;
        pchMessageStart[3] = 0xfd;
        vAlertPubKey = ParseHex("04c1b4a7143a80e71bd6d57e293b3681d652b3ca1d04a1df987db15fe3ed886cc24578faaaf2f00ba216c1000bbbd00c4a63726b8f3370dc8a59949074de1f4f26");
        nDefaultPort = 48848;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 2 * 60;
        nTargetSpacing = 2 * 60;
        nLastPOWBlock = 200;
        nBTChangeBlock = 1000;
        nMaturity = 15;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = 51197;
        nMaxMoneyOut = 43199500 * COIN;
        nZerocoinStartHeight = 50;
        nZerocoinStartTime = 1529726039;
        nBlockEnforceSerialRange = 1; //Enforce serial range starting this block
        nBlockRecalculateAccumulators = ~1; //Trigger a recalculation of accumulators
        nBlockFirstFraudulent = ~1; //First block that bad serials emerged
        nBlockLastGoodCheckpoint = ~1; //Last valid accumulator checkpoint
        genesis.nTime = 1607871600;
        genesis.nNonce = 1450468;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("00000c7631ac91c23591067aaa6d4769f354b34ea74dd91e54754ba147afe1c8"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 55);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 58);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 200);
        // Testnet kfx BIP32 pubkeys
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x2a)(0x12)(0x11).convert_to_container<std::vector<unsigned char> >();
        // Testnet kfx BIP32 prvkeys
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x41)(0x11)(0x1a).convert_to_container<std::vector<unsigned char> >();
        // Testnet kfx BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        strSporkKey = "046e1de0e6a1117f16485c59a34e74e29fa40ca8ac9fdb8c1dc9e440f8bec278c0552fd8123c8494c0e705108281382da54e5c00a911f65b9989576d58a67ed1f6";
        strObfuscationPoolDummyAddress = "KJK3gzNgm9RLsTVshiTdqqywNqo27tPrsr";
        nStartMasternodePayments = 1420837558;
        nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
                                       // here because we only have a 8 block finalization window on testnet

        nStakeMinConfirmations = 30;    // Required number of confirmations
        nStakeMinAmount = 1000 * COIN;  // Minimum required staking amount
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xf1;
        pchMessageStart[1] = 0xf2;
        pchMessageStart[2] = 0xf3;
        pchMessageStart[3] = 0xf4;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 24 * 60 * 60;
        nTargetSpacing = 2 * 60;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1607871600;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 1450468;

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 36210;
        assert(hashGenesisBlock == uint256("00000c7631ac91c23591067aaa6d4769f354b34ea74dd91e54754ba147afe1c8"));

        vFixedSeeds.clear();
        vSeeds.clear();

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 51478;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};

static CChainParams* pCurrentParams = 0;

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
