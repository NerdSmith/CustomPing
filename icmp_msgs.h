

enum class IcmpMsg {
    EchoReply,
    DestinationNetworkUnreachable,
    DestinationHostUnreachable,
    DestinationProtocolUnreachable,
    DestinationPortUnreachable,
    FragmentationRequiredAndDFFlagSet,
    SourceRouteFailed,
    DestinationNetworkUnknown,
    DestinationHostUnknown,
    SourceHostIsolated,
    NetworkAdministrativelyProhibited,
    HostAdministrativelyProhibited,
    NetworkUnreachableForToS,
    HostUnreachableForToS,
    CommunicationAdministrativelyProhibited,
    HostPrecedenceViolation,
    PrecedenceCutoffInEffect,
    SourceQuench,
    RedirectDatagramForTheNetwork,
    RedirectDatagramForTheHost,
    RedirectDatagramForTheToSAndNetwork,
    RedirectDatagramForTheToSHost,
    EchoRequest,
    RouterAdvertisement,
    RouterDiscoverySelectionSolicitation,
    TTLExpiredInTransit,
    FragmentReassemblyTimeExceeded,
    PointerIndicatesTheError,
    MissingRequiredOption,
    BadLength,
    Timestamp,
    TimestampReply,
    InformationRequest,
    InformationReply,
    AddressMaskRequest,
    AddressMaskReply
};